/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __DOUKA_COMMAND_PREDICT__
#define __DOUKA_COMMAND_PREDICT__

#include "douka/io.hh"
#include "douka/plugin_interface.hh"

#include <string>
#include <string_view>
#include <vector>

#include <nlohmann/json.hpp>

namespace douka::command::predict {

inline static constexpr std::string_view name = "predict";
inline static constexpr std::string_view description = "Prediction step for an ensemble model";

struct Args {
  std::string state;
  std::vector<std::string> param;
  std::string plugin;
  std::string plugin_param;
  std::string output = "output";
  bool force = false;
};

struct Param {
  std::string name;
  uint64_t seed;
  uint64_t k;

  std::vector<double> Q;  // Optional

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Param, name, seed, k);

  inline bool validate() const {
    if (name.empty()) {
      std::clog << "no name given" << std::endl;
      return false;
    }

    if (k == 0) {
      std::clog << "no state size given" << std::endl;
      return false;
    }

    if (!Q.empty() && Q.size() != static_cast<std::size_t>(k) &&
        Q.size() != static_cast<std::size_t>(k * k)) {
      std::clog << "invalid size of Q given " << Q.size() << " != " << k << " or " << k * k
                << std::endl;
      return false;
    }

    return true;
  }
};

Args get_args(const int argc, const char *const argv[]);
bool validate(const io::State &state, const Param &param);
bool predict(io::State &state, const Param &param, const PluginInterface::SharedPtr plugin);
int entry(const int argc, const char *const argv[]);
} // namespace douka::command::predict
#endif
