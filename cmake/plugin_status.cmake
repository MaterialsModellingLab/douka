# Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
# SPDX-License-Identifier: Apache-2.0

function(douka_plugin_status TARGET)
  get_target_property(DEFINES ${TARGET} COMPILE_DEFINITIONS)
  message(STATUS "<<< Build configuration >>>
      Version:           ${PROJECT_VERSION}
      Operating System:  ${CMAKE_SYSTEM_NAME} ${CMAKE_LINUX_DISTRO} ${CMAKE_DISTRO_VERSION}
      CMake Version:     ${CMAKE_VERSION}
      Install path:      ${DOUKA_DEFAULT_PLUGIN_PATH}
      Generator:         ${CMAKE_GENERATOR} using ${CMAKE_MAKE_PROGRAM}")

  string(TOUPPER "${CMAKE_BUILD_TYPE}" BTYPE)
  message(STATUS "<<< Compilers and Flags: >>>
      C++ Compiler:      ${CMAKE_CXX_COMPILER}
      Type:              ${CMAKE_CXX_COMPILER_ID}
      Version:           ${CMAKE_CXX_COMPILER_VERSION}
      C++ Flags:         ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_${BTYPE}}
      Defines:           ${DEFINES}")
endfunction()
