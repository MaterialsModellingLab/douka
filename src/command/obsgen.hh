/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __DOUKA_COMMAND_OBSGEN__
#define __DOUKA_COMMAND_OBSGEN__

#include "douka/io.hh"
#include "douka/plugin_interface.hh"

#include <string>
#include <string_view>
#include <vector>

#include <nlohmann/json.hpp>

namespace douka::command::obsgen {
inline static constexpr std::string_view name = "obsgen";
inline static constexpr std::string_view description =
    "Generate observation data for twin experiment";
struct Args {
  std::vector<std::string> param;
  std::string plugin;
  std::string plugin_param;
  std::string output = "output";
  bool force = false;
};

struct Param {
  std::string name;
  uint64_t seed;
  uint64_t t;
  uint64_t k;
  uint64_t l;
  std::vector<double> x0;
  std::vector<double> H; // Optional

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Param, name, seed, t, k, l, x0);

  inline bool validate() const {
    if (name.empty()) {
      std::clog << "no name given" << std::endl;
      return false;
    }

    if (t == 0) {
      std::clog << "no time step given" << std::endl;
      return false;
    }

    if (k == 0) {
      std::clog << "no state size given" << std::endl;
      return false;
    }

    if (l == 0) {
      std::clog << "no observation size given" << std::endl;
      return false;
    }

    if (x0.size() != static_cast<std::size_t>(k)) {
      std::clog << "invalid size of x0 given " << x0.size() << " != " << k << std::endl;
      return false;
    }

    if (!H.empty() && H.size() != static_cast<std::size_t>(k * l)) {
      std::clog << "invalid size of H given " << H.size() << " != " << k * l << std::endl;
      return false;
    }

    return true;
  }
};

Args get_args(const int argc, const char *const argv[]);
bool validate(const Param &param);
bool obsgen(std::vector<io::Obs> &observations, const Param &param,
            const PluginInterface::SharedPtr plugin);
int entry(const int argc, const char *const argv[]);
} // namespace douka::command::obsgen

#endif