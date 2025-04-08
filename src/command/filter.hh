/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __DOUKA_COMMAND_FILTER__
#define __DOUKA_COMMAND_FILTER__

#include <string>
#include <string_view>
#include <vector>

namespace douka::command::filter {
inline static constexpr std::string_view name = "filter";
inline static constexpr std::string_view description = "Filter state vectors with observation data";

struct Args {
  std::string state;
  std::vector<std::string> param;
  std::string obs;
  std::string filter = "enkf";
  std::string output = "output";
  bool force = false;
};

Args get_args(const int argc, const char *const argv[]);
int entry(const int argc, const char *const argv[]);
} // namespace douka::command::filter
#endif
