# Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
# SPDX-License-Identifier: Apache-2.0

# CMake Module Imports
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)


function(douka_install)
  install(TARGETS ${ARGV}
    EXPORT ${PROJECT_NAME}Targets
    PUBLIC_HEADER DESTINATION include/${PROJECT_NAME})

  install(EXPORT ${PROJECT_NAME}Targets
    NAMESPACE ${PROJECT_NAME}::
    FILE ${PROJECT_NAME}Targets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

  configure_package_config_file(
    "cmake/template/config.cmake.in"
    "${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
  write_basic_package_version_file(
    ${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY ExactVersion)

  install(
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

  if (NOT CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    configure_file(
      ${CMAKE_SOURCE_DIR}/cmake/template/setup-env.sh.in
      ${CMAKE_BINARY_DIR}/setup-env.sh
      @ONLY)
    install(
      FILES
      ${CMAKE_CURRENT_BINARY_DIR}/setup-env.sh
      DESTINATION ${CMAKE_INSTALL_PREFIX}/env/${PROJECT_NAME})
  endif()

  # Install Schemas into share directory
  install(
    DIRECTORY
    ${CMAKE_SOURCE_DIR}/schemas
    DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME})
  install(
    FILES
      ${CMAKE_SOURCE_DIR}/cmake/plugin_install.cmake
      ${CMAKE_SOURCE_DIR}/cmake/plugin_status.cmake
      ${CMAKE_SOURCE_DIR}/cmake/filesystem.cmake
    DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/cmake)
endfunction()
