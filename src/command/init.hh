/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __DOUKA_COMMAND_INIT__
#define __DOUKA_COMMAND_INIT__

#include "douka/io.hh"

#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include <nlohmann/json.hpp>

namespace douka::command::init {

inline static constexpr std::string_view name = "init";
inline static constexpr std::string_view description = "Provide initial distribution";

struct Args {
  std::vector<std::string> param;
  std::string output = "output";
  bool force = false;
};

struct Param {
  std::string name;
  uint64_t seed;
  uint64_t N;
  uint64_t k;
  std::vector<double> x0;
  std::vector<double> V0;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Param, name, seed, N, k, x0, V0);

  inline bool validate() const {
    if (name.empty()) {
      std::clog << "no name given" << std::endl;
      return false;
    }

    if (k == 0) {
      std::clog << "no state size given" << std::endl;
      return false;
    }

    if (x0.size() != static_cast<std::size_t>(k)) {
      std::clog << "invalid size of x0 given " << x0.size() << " != " << k << std::endl;
      return false;
    }

    if (V0.size() != static_cast<std::size_t>(k)) {
      std::clog << "invalid size of V0 given " << V0.size() << " != " << k << std::endl;
      return false;
    }

    return true;
  }
};

Args get_args(const int argc, char const *const argv[]);
bool validate(const Param &param);
bool init(std::vector<io::State> &states, const Param &param);
int entry(const int argc, const char *const argv[]);
} // namespace douka::command::init

#endif