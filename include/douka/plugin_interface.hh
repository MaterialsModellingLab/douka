/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __DOUKA__PLUGIN_INTERFACE__
#define __DOUKA__PLUGIN_INTERFACE__

#include <memory>
#include <vector>

namespace douka {
/**
 * @brief Plugin Interface Base Class
 */
class PluginInterface {
public:
  using UniquePtr = std::unique_ptr<PluginInterface>;
  using SharedPtr = std::shared_ptr<PluginInterface>;
  enum class context { none, predict, obsgen };

  // Those members are assigned by the executable.
  // Do not modify them in the plugin.
  int64_t id = -1;
  int64_t sys_tim = -1;
  context ctx = context::none;

public:
  PluginInterface() = default;
  virtual ~PluginInterface() = default;
  PluginInterface(PluginInterface &&other) noexcept = default;
  PluginInterface(const PluginInterface &) = delete;
  PluginInterface &operator=(const PluginInterface &) = delete;
  PluginInterface &operator=(PluginInterface &&other) noexcept = default;

  virtual bool set_option([[maybe_unused]] const std::string &filename) { return true; }
  virtual bool predict(std::vector<double> &state, const std::vector<double> &noise) = 0;
};
} // namespace douka

#endif