/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __DOUKA_FILTER_PARTICLE__
#define __DOUKA_FILTER_PARTICLE__

#include "command/filter.hh"

#include <string_view>

namespace douka::filter::particle {
inline static constexpr std::string_view name = "particle";
inline static constexpr std::string_view description = "Particle Filter";

int entry(const command::filter::Args &args);
} // namespace douka::filter::particle

#endif
