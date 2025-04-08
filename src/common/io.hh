/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __DOUKA_COMMON_IO__
#define __DOUKA_COMMON_IO__

#include "douka/plugin_interface.hh"
#include <filesystem>
#include <string>

namespace douka::io {
bool is_plugin(const std::filesystem::path &real_name);
std::filesystem::path find_plugin(const std::string &name);
PluginInterface::SharedPtr load_plugin(const std::filesystem::path &real_name);
} // namespace douka::io
#endif
