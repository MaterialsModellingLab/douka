/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __DOUKA_COMMAND__
#define __DOUKA_COMMAND__

#include "command/filter.hh"
#include "command/init.hh"
#include "command/obsgen.hh"
#include "command/predict.hh"

#include <string>
#include <string_view>

namespace douka::command {
enum class id { init, predict, filter, obsgen };

inline static const std::string_view names[] = {
    init::name,
    predict::name,
    filter::name,
    obsgen::name,
};

inline static const std::string_view descriptions[] = {
    init::description,
    predict::description,
    filter::description,
    obsgen::description,
};

} // namespace douka::command

#endif