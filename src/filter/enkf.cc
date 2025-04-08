/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#include "enkf.hh"
#include "common/compute.hh"

#include <Eigen/Core>
#include <Eigen/QR>

#include <algorithm>
#include <cassert>
#include <cinttypes>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace douka::filter::enkf {
bool validate(const std::vector<io::State> &states, const io::Obs &obs, const Param &param) {
  if (!std::all_of(states.begin(), states.end(),
                   [](const auto &state) { return state.validate(); })) {
    return false;
  }
  if (!obs.validate()) {
    return false;
  }
  if (!param.validate()) {
    return false;
  }

  if (!std::all_of(states.begin(), states.end(), [param](const auto &state) {
        return state.x.size() == static_cast<std::size_t>(param.k);
      })) {
    std::clog << "invalid state size" << std::endl;
    return false;
  }

  if (static_cast<std::size_t>(param.l) != obs.y.size()) {
    std::clog << "invalid observation size" << std::endl;
    return false;
  }

  if (param.name != obs.name ||
      !std::all_of(states.begin(), states.end(),
                   [name = param.name](const auto &state) { return state.name == name; })) {
    std::clog << "invalid name" << std::endl;
    return false;
  }

  if (!std::all_of(states.begin(), states.end(), [tim = obs.obs_tim](const auto &state) {
        return state.sys_tim == tim && state.obs_tim == tim - 1;
      })) {
    std::clog << "invalid timestamp" << std::endl;
    return false;
  }

  return true;
}

bool filter(std::vector<io::State> &states, const io::Obs &obs, const Param &param) {
  static std::default_random_engine engine{static_cast<unsigned>(param.seed + obs.obs_tim)};
  Eigen::MatrixXd X{param.k, param.N};
  for (const auto &state : states) {
    X.col(state.id) = Eigen::Map<const Eigen::VectorXd>{state.x.data(),
                                                        static_cast<Eigen::Index>(state.x.size())};
  }

  Eigen::MatrixXd H;
  if (param.H.empty()) {
    H = Eigen::MatrixXd::Identity(param.l, param.k);
  } else {
    H = Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>{
        param.H.data(), static_cast<Eigen::Index>(param.l), static_cast<Eigen::Index>(param.k)};
  }

  Eigen::MatrixXd R;
  if (param.R.empty()) {
    R = Eigen::MatrixXd::Zero(param.l, param.l);
  } else if (param.R.size() == static_cast<std::size_t>(param.l)) {
    R = Eigen::Map<const Eigen::VectorXd>{param.R.data(), static_cast<Eigen::Index>(param.l)}
            .asDiagonal();
  } else {
    R = Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>{
        param.R.data(), static_cast<Eigen::Index>(param.l), static_cast<Eigen::Index>(param.l)};
  }

  const bool is_tall = param.N < param.l && param.N < param.k;
  const Eigen::MatrixXd K = is_tall ? common::compute::kalman_gain_tall(X, H, R).eval()
                                    : common::compute::kalman_gain(X, H, R).eval();

  const auto Y =
      Eigen::Map<const Eigen::VectorXd>{obs.y.data(), static_cast<Eigen::Index>(obs.y.size())}
          .replicate(1, param.N);
  const auto W = common::compute::rand(R, param.N, engine);
  const auto X_next = X + K * (Y + common::compute::mean_diff(W) - H * X);

  for (auto &state : states) {
    Eigen::Map<Eigen::VectorXd>{state.x.data(), static_cast<Eigen::Index>(state.x.size())} =
        X_next.col(state.id);
    state.obs_tim++;
  }

  return true;
}

int entry(const command::filter::Args &args) {
  if (!std::filesystem::exists(args.output) && !std::filesystem::create_directories(args.output)) {
    return EXIT_FAILURE;
  }

  /* Parse filename */
  std::vector<std::string> state_files;
  if (!io::parse_filename(args.state, state_files)) {
    return EXIT_FAILURE;
  }
  std::vector<std::string> param_filenames;
  for (const auto &param : args.param) {
    if (!io::parse_filename(param, param_filenames)) {
      return EXIT_FAILURE;
    }
  }
  /* filename -> json */
  std::vector<nlohmann::json> state_jsons;
  state_jsons.reserve(state_files.size());
  for (const auto &state_file : state_files) {
    nlohmann::json state_json;
    if (!io::read_json(state_file, state_json)) {
      return EXIT_FAILURE;
    }
    state_jsons.emplace_back(state_json);
  }
  state_files.clear();
  nlohmann::json obs_json, param_json;
  if (!io::read_json(args.obs, obs_json)) {
    return EXIT_FAILURE;
  }
  for (const auto &param_filename : param_filenames) {
    if (!io::read_json(param_filename, param_json)) {
      return EXIT_FAILURE;
    }
  }
  param_filenames.clear();

  /* json -> object */
  std::vector<io::State> states;
  states.reserve(state_jsons.size());
  for (const auto &state_json : state_jsons) {
    try {
      states.emplace_back(state_json);
    } catch (const nlohmann::json::exception &e) {
      std::clog << "failed to parse state json " << e.what() << std::endl;
      return EXIT_FAILURE;
    }
  }
  state_jsons.clear();
  Param param;
  io::Obs obs;
  try {
    obs = obs_json;
  } catch (const nlohmann::json::exception &e) {
    std::clog << "failed to parse obs json " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  try {
    param = param_json;
  } catch (const nlohmann::json::exception &e) {
    std::clog << "failed to parse param json " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  if (param_json.contains("R") && param_json["R"].is_array()) {
    param.R = param_json["R"].get<std::vector<double>>();
  }
  if (param_json.contains("H") && param_json["H"].is_array()) {
    param.H = param_json["H"].get<std::vector<double>>();
  }

  /* Check integrity */
  if (!validate(states, obs, param)) {
    return EXIT_FAILURE;
  }

  if (!filter(states, obs, param)) {
    return EXIT_FAILURE;
  }

  for (const auto &state : states) {
    const auto &filename = std::filesystem::path(args.output) / io::state_filename(state);
    if (!io::write_json(filename, state, args.force)) {
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}
} // namespace douka::filter::enkf
