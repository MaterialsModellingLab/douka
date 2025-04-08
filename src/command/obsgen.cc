/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#include "obsgen.hh"
#include "common/compute.hh"
#include "common/io.hh"

#include <Eigen/Core>
#include <Eigen/QR>

#include <cinttypes>
#include <filesystem>
#include <random>

namespace douka::command::obsgen {
static bool show_help(const int argc, char const *const argv[]) {
  static const auto &show_help = [argv](std::ostream &os) {
    os << argv[0] << " " << argv[1] << " [Options]" << std::endl;
    os << "Description:" << std::endl;
    os << "   " << description << std::endl;
    os << std::endl;
    os << "Options:" << std::endl;
    os << "   --param           Input parameter json files" << std::endl;
    os << "   --plugin          System model plugin" << std::endl;
    os << "   --plugin_option   (Opt) Plugin option json file" << std::endl;
    os << "   --output          (Opt) Output path (default='output')" << std::endl;
    os << "   --force           (Opt) Overwrite existing file" << std::endl;
    os << "   --help            (Opt) Print help message" << std::endl;
  };

  if (argc <= 2) {
    show_help(std::cout);
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

Args get_args(const int argc, char const *const argv[]) {
  Args args;
  enum class Context {
    none = 0,
    param,
    plugin,
    plugin_param,
    output,
  } ctx = Context::none;

  for (int i = 2; i < argc; i++) {
    if (!strncmp(argv[i], "--", 2)) {
      ctx = Context::none;
      if (!strcmp(argv[i], "--param")) {
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
  if (args.param.empty()) {
    throw std::invalid_argument("required option '--param' not given");
  }
  if (args.plugin.empty()) {
    throw std::invalid_argument("required option '--plugin' not given");
  }
  return args;
}

bool validate(const Param &param) {
  if (!param.validate()) {
    return false;
  }

  return true;
}

bool obsgen(std::vector<io::Obs> &observations, const Param &param,
            const PluginInterface::SharedPtr plugin) {
  static std::default_random_engine engine{static_cast<unsigned>(param.seed)};

  observations.resize(param.t + 1);

  Eigen::MatrixXd H;
  if (param.H.empty()) {
    H = Eigen::MatrixXd::Identity(param.l, param.k);
  } else {
    H = Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>{
        param.H.data(), static_cast<Eigen::Index>(param.l), static_cast<Eigen::Index>(param.k)};
  }

  std::vector<double> x_data = param.x0;
  auto x = Eigen::Map<Eigen::VectorXd>(x_data.data(), x_data.size());
  const std::vector<double> zeros(param.k, 0.0);

  int64_t t = 0;
  for (auto &obs : observations) {
    plugin->sys_tim = t;

    obs.name = param.name;
    obs.obs_tim = t;
    obs.y.resize(param.l);
    auto y = Eigen::Map<Eigen::VectorXd>{obs.y.data(), static_cast<Eigen::Index>(obs.y.size())};

    y = H * x;

    if (!plugin->predict(x_data, zeros)) {
      return false;
    }
    t++;
  }

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

  /* Parse filename */
  std::vector<std::string> param_filenames;
  for (const auto &param : args.param) {
    if (!io::parse_filename(param, param_filenames)) {
      return EXIT_FAILURE;
    }
  }

  /* filename -> json */
  nlohmann::json param_json;
  for (const auto &filename : param_filenames) {
    if (!io::read_json(filename, param_json)) {
      return EXIT_FAILURE;
    }
  }
  param_filenames.clear();

  /* json -> object */
  Param param;
  try {
    param = param_json;
  } catch (const nlohmann::json::exception &e) {
    std::clog << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  if (param_json.contains("H") && param_json["H"].is_array()) {
    param.H = param_json["H"].get<std::vector<double>>();
  }

  if (!validate(param)) {
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

  plugin->ctx = PluginInterface::context::obsgen;
  if (!args.plugin_param.empty() && !std::filesystem::exists(args.plugin_param)) {
    std::clog << args.plugin_param << " not exist" << std::endl;
    return EXIT_FAILURE;
  }
  if (!plugin->set_option(args.plugin_param)) {
    return EXIT_FAILURE;
  }

  std::vector<io::Obs> observations;
  if (!obsgen(observations, param, plugin)) {
    return EXIT_FAILURE;
  }

  for (const auto &obs : observations) {
    const auto &filename = std::filesystem::path(args.output) / io::obs_filename(obs);
    if (!io::write_json(filename, obs, args.force)) {
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}
} // namespace douka::command::obsgen
