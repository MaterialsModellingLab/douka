/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#include "init.hh"
#include "common/compute.hh"
#include "common/io.hh"

#include <Eigen/Core>

#include <algorithm>
#include <filesystem>
#include <random>
#include <unordered_map>

namespace douka::command::init {
static bool show_help(const int argc, char const *const argv[]) {

  static const auto &show_help = [argv](std::ostream &os) {
    os << argv[0] << " " << argv[1] << " [Options]" << std::endl;
    os << "Description:" << std::endl;
    os << "   " << description << std::endl;
    os << std::endl;
    os << "Options:" << std::endl;
    os << "   --param       Input parameter json files" << std::endl;
    os << "   --output      (Opt) Output path (default='output')" << std::endl;
    os << "   --force       (Opt) Overwrite existing file" << std::endl;
    os << "   --help        (Opt) Print help message" << std::endl;
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
    output,
  } ctx = Context::none;

  for (int i = 2; i < argc; i++) {
    if (!strncmp(argv[i], "--", 2)) {
      ctx = Context::none;
      if (!strcmp(argv[i], "--param")) {
        ctx = Context::param;
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
  return args;
}

bool validate(const Param &param) {
  if (!param.validate()) {
    return false;
  }

  return true;
}

bool init(std::vector<io::State> &states, const Param &param) {
  static std::default_random_engine engine{static_cast<unsigned>(param.seed)};
  const Eigen::VectorXd V = Eigen::Map<const Eigen::VectorXd>(param.V0.data(), param.V0.size());
  const auto x0 = Eigen::Map<const Eigen::VectorXd>(param.x0.data(), param.x0.size());
  states.reserve(param.N);

  for (uint64_t i = 0; i < param.N; ++i) {
    io::State state;
    state.name = param.name;
    state.id = i;
    state.obs_tim = 0;
    state.sys_tim = 0;

    state.x.resize(param.k);
    auto x = Eigen::Map<Eigen::VectorXd>(state.x.data(), state.x.size());
    x = common::compute::rand(V, 1, engine) + x0;

    states.emplace_back(state);
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

  if (!validate(param)) {
    return EXIT_FAILURE;
  }

  std::vector<io::State> states;
  if (!init(states, param)) {
    std::clog << "failed to initialize" << std::endl;
    return EXIT_FAILURE;
  }

  for (const auto &state : states) {
    const auto &filename = std::filesystem::path(args.output) / io::state_filename(state);
    if (!io::write_json(filename.string(), state, args.force)) {
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}
} // namespace douka::command::init