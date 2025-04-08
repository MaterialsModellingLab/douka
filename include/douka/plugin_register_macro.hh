/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __DOUKA__PLUGIN_REGISTER_MACRO__
#define __DOUKA__PLUGIN_REGISTER_MACRO__

#include <douka/plugin_interface.hh>

#define DOUKA_PLUGIN_REGISTER(__func__)                                                            \
  extern "C" {                                                                                     \
  douka::PluginInterface *make() { return new __func__; }                                          \
  }

#endif