/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#include "douka/plugin_interface.hh"
#include <cassert>
#include <cmath>
#include <iostream>

class SamplePlugin : public douka::PluginInterface {
public:
  bool predict([[maybe_unused]] std::vector<double> &state, //
               [[maybe_unused]] const std::vector<double> &noise) override {
    assert(this->id != -1);
    assert(this->sys_tim != -1);

    assert(state.at(0) == 1.0);
    assert(state.at(1) == 2.0);
    assert(state.at(2) == 3.0);

    assert(!std::isnan(noise.at(0)));
    assert(!std::isnan(noise.at(1)));
    assert(!std::isnan(noise.at(2)));

    state[0] += noise[0];
    state[1] += noise[1];
    state[2] += noise[2];

    return true;
  }
};

// Missing registration

// #include "douka/plugin_register_macro.hh"
// DOUKA_PLUGIN_REGISTER(SamplePlugin)
