/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __DOUKA__IO__
#define __DOUKA__IO__

#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

namespace douka::io {
struct State {
  std::string name;
  int64_t id;
  int64_t sys_tim;
  int64_t obs_tim;
  std::vector<double> x;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(State, name, id, sys_tim, obs_tim, x);

  inline bool validate() const {
    if (this->name.empty()) {
      std::clog << "No name given" << std::endl;
      return false;
    }
    if (this->id < 0) {
      std::clog << "Invalid id given " << this->id << std::endl;
      return false;
    }
    if (this->sys_tim < 0) {
      std::clog << "Invalid sys time given " << this->sys_tim << std::endl;
      return false;
    }
    if (this->obs_tim < 0) {
      std::clog << "Invalid obs time given " << this->obs_tim << std::endl;
      return false;
    }
    if (this->x.empty()) {
      std::clog << "No state given" << std::endl;
      return false;
    }
    return true;
  }

  inline friend std::ostream &operator<<(std::ostream &os, const State &state) {
    os << "name: " << state.name << ", ";
    os << "id: " << state.id << ", ";
    os << "sys_tim: " << state.sys_tim << ", ";
    os << "obs_tim: " << state.obs_tim << ", ";
    os << "x: [";
    for (std::size_t i = 0; i < state.x.size(); ++i) {
      os << state.x[i];
      if (i != state.x.size() - 1) {
        os << ", ";
      }
    }
    os << "]";
    return os;
  }
};

struct Obs {
  std::string name;
  int64_t obs_tim;
  std::vector<double> y;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Obs, name, obs_tim, y);

  inline bool validate() const {
    if (this->name.empty()) {
      std::clog << "No name given" << std::endl;
      return false;
    }
    if (this->obs_tim < 0) {
      std::clog << "Invalid obs time given " << this->obs_tim << std::endl;
      return false;
    }
    if (this->y.empty()) {
      std::clog << "No observation given" << std::endl;
      return false;
    }
    return true;
  }

  inline friend std::ostream &operator<<(std::ostream &os, const Obs &obs) {
    os << "name: " << obs.name << ", ";
    os << "obs_tim: " << obs.obs_tim << ", ";
    os << "y: [";
    for (std::size_t i = 0; i < obs.y.size(); ++i) {
      os << obs.y[i];
      if (i != obs.y.size() - 1) {
        os << ", ";
      }
    }
    os << "]";
    return os;
  }
};

inline bool read_json(const std::filesystem::path &filename, nlohmann::json &json) {
  if (!std::filesystem::exists(filename)) {
    std::clog << filename << " not exists" << std::endl;
    return false;
  }

  if (!std::filesystem::is_regular_file(filename)) {
    std::clog << filename << " is not a regular file" << std::endl;
    return false;
  }

  std::ifstream stream{filename};
  if (!stream) {
    std::cerr << filename << " could not open" << std::endl;
    return false;
  }
  try {
    const auto tmp = nlohmann::json::parse(stream);
    json.update(tmp);
  } catch (const nlohmann::json::parse_error &e) {
    std::clog << filename << ": " << e.what() << std::endl;
    return false;
  }
  return true;
}

inline bool write_json(const std::filesystem::path &filename, const nlohmann::json &json,
                       const bool force = false) {
  if (!force && std::filesystem::exists(filename) && std::filesystem::is_regular_file(filename)) {
    std::clog << filename << " already exists" << std::endl;
    return false;
  }

  std::ofstream stream{filename};
  if (!stream) {
    std::cerr << filename << " could not open" << std::endl;
    return false;
  }
  stream << std::setw(2) << json << std::endl;
  return true;
}

inline bool parse_filename(const std::string &input, std::vector<std::string> &files) {
  const auto regex_match_count = [](const std::string &s, const std::regex &pattern) -> int64_t {
    const auto begin = std::sregex_iterator(s.begin(), s.end(), pattern);
    const auto end = std::sregex_iterator();
    return std::distance(begin, end);
  };
  const int64_t max_ensemble = 10000;
  const auto yield_filename = [](const std::string &s, const int64_t i, std::string &out) -> bool {
    out.resize(s.size() + std::log10(max_ensemble) + 1);
    const int rc = snprintf(out.data(), out.size(), s.c_str(), i);
    if (rc < 0) {
      std::clog << "snprintf status error " << rc << std::endl;
      return false;
    }
    return true;
  };

  const auto check_exists = [](const std::filesystem::path &filename) -> bool {
    if (!std::filesystem::exists(filename)) {
      std::clog << filename << " not exists" << std::endl;
      return false;
    }
    return true;
  };

  std::regex pattern{R"(%(\d+|)d)"};
  const int64_t match_cnt = regex_match_count(input, pattern);
  if (match_cnt == 0) {
    if (!check_exists(input)) {
      return false;
    }
    files.emplace_back(input);
  } else if (match_cnt == 1) {
    int64_t start;
    std::string filename;
    for (start = 0; start < max_ensemble; ++start) {
      if (!yield_filename(input, start, filename)) {
        return false;
      }
      if (!std::filesystem::exists(filename)) {
        continue;
      }
      files.emplace_back(filename);
      ++start;
      break;
    }
    for (; start < max_ensemble; ++start) {
      if (!yield_filename(input, start, filename)) {
        return false;
      }
      if (std::filesystem::exists(filename)) {
        files.emplace_back(filename);
        continue;
      }
      break;
    }
    if (files.empty()) {
      std::clog << input << " does not match to any file" << std::endl;
      return false;
    }
  } else {
    std::clog << input << " only 1 digit placeholder allowed but found " << match_cnt << std::endl;
    return false;
  }
  return true;
}

inline std::string state_filename(const State &state) {
  std::stringstream ss;
  ss << state.name << "_";
  ss << std::setfill('0') << std::setw(4) << state.id << "_";
  ss << std::setfill('0') << std::setw(6) << state.sys_tim << "_";
  ss << std::setfill('0') << std::setw(6) << state.obs_tim << ".json";
  return ss.str();
}

inline std::string state_filename_with_id_place_holder(const State &state) {
  std::stringstream ss;
  ss << state.name << "_%04d_";
  ss << std::setfill('0') << std::setw(6) << state.sys_tim << "_";
  ss << std::setfill('0') << std::setw(6) << state.obs_tim << ".json";
  return ss.str();
}

inline std::string obs_filename(const Obs &obs) {
  std::stringstream ss;
  ss << obs.name << "_obs_";
  ss << std::setfill('0') << std::setw(6) << obs.obs_tim << ".json";
  return ss.str();
}
} // namespace douka::io
#endif
