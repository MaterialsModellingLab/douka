# Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
# SPDX-License-Identifier: Apache-2.0

# CMake Module Imports
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

function(douka_plugin_install)
  install(TARGETS ${ARGV} LIBRARY DESTINATION ${DOUKA_DEFAULT_PLUGIN_PATH})
endfunction()
