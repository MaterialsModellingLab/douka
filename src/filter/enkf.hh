/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __DOUKA_FILTER_ENKF__
#define __DOUKA_FILTER_ENKF__

#include "command/filter.hh"
#include "common/io.hh"
#include "douka/io.hh"

#include <Eigen/Core>

#include <nlohmann/json.hpp>

#include <iostream>
#include <vector>

namespace douka::filter::enkf {
inline static constexpr std::string_view name = "enkf";
inline static constexpr std::string_view description = "Ensemble Kalman Filter.";

struct Param {
  std::string name;
  int64_t seed;
  int64_t N;
  int64_t k;
  int64_t l;

  std::vector<double> R; // Optional
  std::vector<double> H; // Optional

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Param, name, seed, N, k, l);

  inline bool validate() const {
    if (name.empty()) {
      std::clog << "no name given" << std::endl;
      return false;
    }
    if (N == 0) {
      std::clog << "no ensemble size given" << std::endl;
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
    if (!R.empty() && R.size() != static_cast<std::size_t>(l) &&
        R.size() != static_cast<std::size_t>(l * l)) {
      std::clog << "invalid size of R given " << R.size() << " != " << l << " or " << l * l
                << std::endl;
      return false;
    }
    if (!H.empty() && H.size() != static_cast<std::size_t>(k * l)) {
      std::clog << "invalid size of H given " << H.size() << " != " << k * l << std::endl;
      return false;
    }
    return true;
  }
};

bool validate(const std::vector<io::State> &states, const io::Obs &obs, const Param &param);
bool filter(std::vector<io::State> &states, const io::Obs &obs, const Param &param);
int entry(const command::filter::Args &args);
} // namespace douka::filter::enkf

#endif
