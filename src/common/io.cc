/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#include "io.hh"

#include <dlfcn.h>
#include <iostream>
#include <string_view>

namespace douka::io {
#if (defined(WIN32) || defined(WIN64))
static constexpr std::string_view plugin_prefix = "";
static constexpr std::string_view plugin_ext = ".dll";
#elif defined(__APPLE__)
static constexpr std::string_view plugin_prefix = "lib";
static constexpr std::string_view plugin_ext = ".dylib";
#elif defined(__linux__)
static constexpr std::string_view plugin_prefix = "lib";
static constexpr std::string_view plugin_ext = ".so";
#endif

#if defined(DOUKA_DEFAULT_PLUGIN_PATH)
static constexpr std::string_view default_plugin_path = DOUKA_DEFAULT_PLUGIN_PATH;
#endif

static std::vector<std::filesystem::path> get_plugin_locations() {
  static const std::string_view delimiter = ":";
  std::vector<std::filesystem::path> ret;
  const char *plugin_path_env = std::getenv("DOUKA_PLUGIN_PATH");
  if (plugin_path_env != nullptr) {
    std::size_t pos = 0;
    std::string token = plugin_path_env;
    while ((pos = token.find(delimiter)) != std::string::npos) {
      ret.emplace_back(token.substr(0, pos));
      token.erase(0, pos + delimiter.length());
    }
    ret.emplace_back(token);
  }

  ret.emplace_back(default_plugin_path);
  return ret;
}

bool is_plugin(const std::filesystem::path &real_name) {
  if (real_name.extension() != plugin_ext) {
    return false;
  }
  if (real_name.filename().string().find(plugin_prefix) != 0) {
    return false;
  }
  if (!std::filesystem::exists(real_name)) {
    return false;
  }
  if (!std::filesystem::is_regular_file(real_name)) {
    return false;
  }

  return true;
}

std::filesystem::path find_plugin(const std::string &name) {
  const auto plugin_name = std::string(plugin_prefix) + name + std::string(plugin_ext);

  for (const auto &plugin_path : get_plugin_locations()) {
    const auto real_name = plugin_path / plugin_name;
    if (is_plugin(real_name)) {
      return real_name;
    }
  }

  std::clog << "plugin not found in following paths: " << std::endl;
  for (const auto &plugin_path : get_plugin_locations()) {
    std::clog << "  " << plugin_path << std::endl;
  }

  throw std::runtime_error("plugin not found");
}

typedef PluginInterface *PluginInterfaceCreateFunction();
PluginInterface::SharedPtr load_plugin(const std::filesystem::path &real_name) {
  if (!is_plugin(real_name)) {
    throw std::runtime_error("plugin not found");
  }

  const auto plugin_lib = dlopen(real_name.c_str(), RTLD_LAZY);
  if (plugin_lib == nullptr) {
    throw std::runtime_error("dlopen failed");
  }

  auto plugin_generator =
      reinterpret_cast<PluginInterfaceCreateFunction *>(dlsym(plugin_lib, "make"));
  if (plugin_generator == nullptr) {
    std::clog << "The plugin might not be registered by 'DOUKA_PLUGIN_REGISTER' Macro?"
              << std::endl;
    throw std::runtime_error("dlsym failed");
  }
  return PluginInterface::SharedPtr(plugin_generator());
}
} // namespace douka::io
