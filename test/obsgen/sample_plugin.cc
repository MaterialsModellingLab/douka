/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#include "douka/plugin_interface.hh"
#include <cassert>
#include <iostream>

class SamplePlugin : public douka::PluginInterface {
public:
  bool predict([[maybe_unused]] std::vector<double> &state,
               [[maybe_unused]] const std::vector<double> &noise) override {
    assert(this->sys_tim != -1);
    assert(this->ctx == douka::PluginInterface::context::obsgen);
    assert(noise[0] == 0.0);
    assert(noise[1] == 0.0);
    assert(noise[2] == 0.0);

    return true;
  }
};

#include "douka/plugin_register_macro.hh"
DOUKA_PLUGIN_REGISTER(SamplePlugin)
