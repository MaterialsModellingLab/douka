# Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
# SPDX-License-Identifier: Apache-2.0

find_package(GTest REQUIRED)
include(GoogleTest)

function(add_cli_target TEST_NAME)
  set(target ${CMAKE_CURRENT_SOURCE_DIR}/cli/${TEST_NAME}.sh ${ARGN})
  add_test(NAME "cli-${TEST_NAME}"
    COMMAND bash -x ${target}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endfunction()

function(add_gtest_target COMMAND_NAME TEST_NAME)
  set(TARGET "test-${COMMAND_NAME}-${TEST_NAME}")
  add_executable(${TARGET})
  target_sources(${TARGET}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/${COMMAND_NAME}/${TEST_NAME}.cc)
  target_include_directories(${TARGET}
    PRIVATE
    ${GTEST_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/src)
  target_link_libraries(${TARGET} ${PROJECT_NAME}-static GTest::GTest GTest::Main)
  gtest_discover_tests(${TARGET})
endfunction()

function(add_plugin COMMAND_NAME PLUGIN_NAME)
  set(TARGET "${COMMAND_NAME}-${PLUGIN_NAME}")
  add_library(${TARGET} SHARED)
  target_sources(${TARGET}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/${COMMAND_NAME}/${PLUGIN_NAME}.cc)
  target_link_libraries(${TARGET} plugin_interface)
endfunction()

# Base Command
add_cli_target("entry")
add_cli_target("version")
add_cli_target("help")

# Init Command
add_cli_target("init-help")
add_cli_target("init-valid1")
add_cli_target("init-valid2")
add_cli_target("init-invalid1")

# Filter Command
add_cli_target("filter-help")
add_cli_target("filter-valid1")
add_cli_target("filter-valid2")
add_cli_target("filter-invalid1")

# Predict Command
# Create Plugin
add_plugin("predict" "sample_plugin")
add_plugin("predict" "sample_invalid_plugin")

add_cli_target("predict-help")
add_cli_target("predict-valid1" ${CMAKE_CURRENT_BINARY_DIR}/libpredict-sample_plugin${CMAKE_SHARED_LIBRARY_SUFFIX})
add_cli_target("predict-valid2" ${CMAKE_CURRENT_BINARY_DIR}/libpredict-sample_plugin${CMAKE_SHARED_LIBRARY_SUFFIX})
add_cli_target("predict-invalid1" ${CMAKE_CURRENT_BINARY_DIR}/libpredict-sample_invalid_plugin${CMAKE_SHARED_LIBRARY_SUFFIX})

# Obs gen
# Create Plugin
add_plugin("obsgen" "sample_plugin")

add_cli_target("obsgen-help")
add_cli_target("obsgen-valid1" ${CMAKE_CURRENT_BINARY_DIR}/libobsgen-sample_plugin${CMAKE_SHARED_LIBRARY_SUFFIX})

# GTest
add_gtest_target("common" "compute")
add_gtest_target("filter" "enkf")
add_gtest_target("init" "init")
add_gtest_target("obsgen" "obsgen")
add_gtest_target("predict" "predict")

add_gtest_target("command" "init")
add_gtest_target("command" "filter")
add_gtest_target("command" "predict")
add_gtest_target("command" "obsgen")
