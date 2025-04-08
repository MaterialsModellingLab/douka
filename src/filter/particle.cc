/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#include "particle.hh"

#include <cassert>
#include <iostream>
#include <vector>

namespace douka::filter::particle {
int entry([[maybe_unused]] const command::filter::Args &args) {
  // TODO(m12watanabe1a): Implement it
  std::clog << "filter '" << args.filter << "' not supported yet" << std::endl;
  return EXIT_FAILURE;
}
} // namespace douka::filter::particle
