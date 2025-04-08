/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#include "filter.hh"
#include "common/io.hh"
#include "douka/io.hh"
#include "filter/enkf.hh"
#include "filter/particle.hh"

#include <algorithm>
#include <iostream>
#include <numeric>

namespace douka::command::filter {
static const std::string_view type_names[] = {
    douka::filter::enkf::name,
    douka::filter::particle::name,
};

static bool show_help(const int argc, char const *const argv[]) {
  static const auto &show_help = [argv](std::ostream &os) {
    // NOTE: This could be implemented within the compile time.
    const auto &show_filter_types = []() -> std::string {
      return std::accumulate(
          std::begin(type_names) + 1, std::end(type_names), std::string(type_names[0]),
          [](const std::string_view &s0, const std::string_view &s1) -> std::string {
            return std::string(s0) + "|" + std::string(s1);
          });
    };

    os << argv[0] << " " << argv[1] << " [Options]" << std::endl;
    os << "Description:" << std::endl;
    os << "   " << description << std::endl;
    os << std::endl;
    os << "Options:" << std::endl;
    os << "   --state       Input state vector json file" << std::endl;
    os << "   --param       Input parameter json files" << std::endl;
    os << "   --obs         Input observation json file" << std::endl;
    os << "   --filter      (Opt) Filter [" << show_filter_types() << "] (default=enkf)"
       << std::endl;
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

Args get_args(const int argc, const char *const argv[]) {
  Args args;

  enum class Context {
    none = 0,
    state,
    param,
    obs,
    filter,
    output,
  } ctx = Context::none;

  for (int i = 2; i < argc; i++) {
    if (!strncmp(argv[i], "--", 2)) {
      ctx = Context::none;
      if (!strcmp(argv[i], "--state")) {
        ctx = Context::state;
      } else if (!strcmp(argv[i], "--param")) {
        ctx = Context::param;
      } else if (!strcmp(argv[i], "--obs")) {
        ctx = Context::obs;
      } else if (!strcmp(argv[i], "--filter")) {
        ctx = Context::filter;
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
      case Context::obs: {
        args.obs = argv[i];
        ctx = Context::none;
        break;
      }
      case Context::filter: {
        args.filter = argv[i];
        ctx = Context::none;
        break;
      }
      case Context::output: {
        args.output = argv[i];
        ctx = Context::none;
        break;
      }
      default: {
        throw std::invalid_argument("invalid command '" + std::string{argv[i]} + "' given");
      }
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
  if (args.obs.empty()) {
    throw std::invalid_argument("required option '--obs' not given");
  }

  return args;
}

int entry(const int argc, const char *const argv[]) {
  if (show_help(argc, argv)) {
    return EXIT_SUCCESS;
  }
  const auto args = get_args(argc, argv);

  if (args.filter == "enkf") {
    return douka::filter::enkf::entry(args);
  } else if (args.filter == "particle") {
    return douka::filter::particle::entry(args);
  } else {
    std::clog << "invalid filter '" << args.filter << "' given" << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
} // namespace douka::command::filter
