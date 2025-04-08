/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#include "predict.hh"
#include "common/compute.hh"
#include "common/io.hh"

#include <Eigen/Core>
#include <Eigen/QR>

#include <cinttypes>
#include <filesystem>
#include <random>

namespace douka::command::predict {
static bool show_help(const int argc, char const *const argv[]) {
  static const auto &show_help = [argv](std::ostream &os) {
    os << argv[0] << " " << argv[1] << " [Options]" << std::endl;
    os << "Description:" << std::endl;
    os << "   " << description << std::endl;
    os << std::endl;
    os << "Options:" << std::endl;
    os << "   --state         Input state vector json file" << std::endl;
    os << "   --param         Input parameter json files" << std::endl;
    os << "   --plugin        System model plugin" << std::endl;
    os << "   --plugin_param  (Opt) Plugin option json file" << std::endl;
    os << "   --output        (Opt) Output path (default='output')" << std::endl;
    os << "   --force         (Opt) Overwrite existing file" << std::endl;
    os << "   --help          (Opt) Print help message" << std::endl;
  };

  if (argc <= 2) {
    show_help(std::clog);
    throw std::invalid_argument("no option given");
  }
  for (int i = 2; i < argc; i++) {
    if (!strcmp(argv[i], "--help")) {
      show_help(std::cout);
      return true;
    }
  }
  return false;
}
Args get_args(const int argc, const char *const argv[]) {
  Args args;
  enum class Context {
    none = 0,
    state,
    param,
    plugin,
    plugin_param,
    output,
  } ctx = Context::none;

  for (int i = 2; i < argc; i++) {
    if (!strncmp(argv[i], "--", 2)) {
      ctx = Context::none;
      if (!strcmp(argv[i], "--state")) {
        ctx = Context::state;
      } else if (!strcmp(argv[i], "--param")) {
        ctx = Context::param;
      } else if (!strcmp(argv[i], "--plugin")) {
        ctx = Context::plugin;
      } else if (!strcmp(argv[i], "--plugin_param")) {
        ctx = Context::plugin_param;
      } else if (!strcmp(argv[i], "--output")) {
        ctx = Context::output;
      } else if (!strcmp(argv[i], "--force")) {
        args.force = true;
        ctx = Context::none;
      } else {
        throw std::invalid_argument("unknown option '" + std::string{argv[i]} + "' given");
      }
    } else {
      switch (ctx) {
      case Context::state: {
        args.state = argv[i];
        ctx = Context::none;
        break;
      }
      case Context::param: {
        args.param.emplace_back(argv[i]);
        break;
      }
      case Context::plugin: {
        args.plugin = argv[i];
        ctx = Context::none;
        break;
      }
      case Context::plugin_param: {
        args.plugin_param = argv[i];
        ctx = Context::none;
        break;
      }
      case Context::output: {
        args.output = argv[i];
        ctx = Context::none;
        break;
      }
      default:
        throw std::invalid_argument("invalid command '" + std::string{argv[i]} + "' given");
      }
    }
  }
  if (ctx != Context::none) {
    throw std::invalid_argument("required option for '" + std::string{argv[argc - 1]} +
                                "' not given");
  }
  if (args.state.empty()) {
    throw std::invalid_argument("required option '--state' not given");
  }
  if (args.param.empty()) {
    throw std::invalid_argument("required option '--param' not given");
  }
  if (args.plugin.empty()) {
    throw std::invalid_argument("required option '--plugin' not given");
  }
  return args;
}

bool validate(const io::State &state, const Param &param) {
  if (!param.validate()) {
    return false;
  }

  if (state.x.size() != static_cast<std::size_t>(param.k)) {
    std::clog << "invalid state size" << std::endl;
    return false;
  }

  if (state.name != param.name) {
    std::clog << "invalid name" << std::endl;
    return false;
  }

  return true;
}

bool predict(io::State &state, const Param &param, const PluginInterface::SharedPtr plugin) {
  // To prevent using the same seed in the each ensemble,
  // add its id and sys_tim
  static std::default_random_engine engine{
      static_cast<unsigned>(param.seed + state.id + state.sys_tim)};

  std::vector<double> noise_data;
  if (!param.Q.empty()) {
    noise_data.resize(param.k);
    auto noise = Eigen::Map<Eigen::VectorXd>{noise_data.data(),
                                             static_cast<Eigen::Index>(noise_data.size())};

    Eigen::MatrixXd Q;
    if (param.Q.size() == static_cast<std::size_t>(param.k)) {
      Q = Eigen::Map<const Eigen::VectorXd>{param.Q.data(), static_cast<Eigen::Index>(param.k)}
              .asDiagonal();
    } else if (param.Q.size() == static_cast<std::size_t>(param.k * param.k)) {
      Q = Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>{
          param.Q.data(), static_cast<Eigen::Index>(param.k), static_cast<Eigen::Index>(param.k)};
    }
    noise = common::compute::rand(Q, 1, engine);
  }

  if (!plugin->predict(state.x, noise_data)) {
    return false;
  }
  state.sys_tim++;
  return true;
}

int entry(const int argc, const char *const argv[]) {
  if (show_help(argc, argv)) {
    return EXIT_SUCCESS;
  }
  const auto args = get_args(argc, argv);

  if (!std::filesystem::exists(args.output) && !std::filesystem::create_directories(args.output)) {
    return EXIT_FAILURE;
  }

  /* Parse filename  */
  std::vector<std::string> param_filenames;
  for (const auto &param : args.param) {
    if (!io::parse_filename(param, param_filenames)) {
      return EXIT_FAILURE;
    }
  }

  /* filename -> json */
  nlohmann::json state_json;
  if (!io::read_json(args.state, state_json)) {
    return EXIT_FAILURE;
  }
  nlohmann::json param_json;
  for (const auto &param_filename : param_filenames) {
    if (!io::read_json(param_filename, param_json)) {
      return EXIT_FAILURE;
    }
  }
  param_filenames.clear();

  /* json -> object */
  io::State state;
  Param param;
  try {
    state = state_json;
    param = param_json;
  } catch (const nlohmann::json::exception &e) {
    std::clog << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  if (param_json.contains("Q") && param_json["Q"].is_array()) {
    param.Q = param_json["Q"].get<std::vector<double>>();
  }

  if (!validate(state, param)) {
    return EXIT_FAILURE;
  }

  /* Load plugin */
  PluginInterface::SharedPtr plugin;
  try {
    const auto plugin_name = io::is_plugin(args.plugin) ? std::filesystem::path(args.plugin)
                                                        : io::find_plugin(args.plugin);
    plugin = io::load_plugin(plugin_name);
  } catch (const std::runtime_error &e) {
    std::clog << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  plugin->id = state.id;
  plugin->sys_tim = state.sys_tim;
  plugin->ctx = PluginInterface::context::predict;
  if (!args.plugin_param.empty() && !std::filesystem::exists(args.plugin_param)) {
    std::clog << args.plugin_param << " not exist" << std::endl;
    return EXIT_FAILURE;
  }
  if (!plugin->set_option(args.plugin_param)) {
    return EXIT_FAILURE;
  }

  /* Run prediction */
  if (!predict(state, param, plugin)) {
    return EXIT_FAILURE;
  }

  const auto &filename = std::filesystem::path(args.output) / io::state_filename(state);
  if (!io::write_json(filename.string(), state, args.force)) {
    return EXIT_FAILURE;
  }
  std::cout << "result saved to " << filename << std::endl;

  return EXIT_SUCCESS;
}
} // namespace douka::command::predict
