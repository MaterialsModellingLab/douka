# Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.27 FATAL_ERROR)
project(douka LANGUAGES CXX DESCRIPTION "Data Assimilattion Library for CPP")

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.local/" CACHE PATH "Default install path" FORCE)
endif()

# Options
option(DOUKA_USE_SANITIZER "Build with sanitizer" OFF)
option(DOUKA_USE_MKL "Use Intel MKL" OFF)
option(DOUKA_USE_BLAS "Use Lapacke" OFF)
option(BUILD_DOC "Build documentation" OFF)
option(BUILD_TESTING "Build unit tests" OFF)
option(BUILD_BENCHMARK "Build benchmark" OFF)

# Define executable name for different platforms
set(DOUKA_EXECUTABLE_NAME "${PROJECT_NAME}")

# CMake Local Module Imports
include(${CMAKE_SOURCE_DIR}/cmake/clang_format.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/cppcheck.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/dependencies.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/filesystem.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/git-version.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/install.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/status.cmake)

# Get git tag as PROJECT_VERSION
get_git_version()

# Build Documentation
if(BUILD_DOC)
  include(${CMAKE_SOURCE_DIR}/cmake/doc.cmake)
  build_doc()
  return()
endif(BUILD_DOC)

# Interface
set(PLUGIN_INTERFACE_PUBLIC_HEADERES
  ${CMAKE_SOURCE_DIR}/include/${PROJECT_NAME}/plugin_interface.hh
  ${CMAKE_SOURCE_DIR}/include/${PROJECT_NAME}/plugin_register_macro.hh
  ${CMAKE_SOURCE_DIR}/include/${PROJECT_NAME}/io.hh)
add_library(plugin_interface INTERFACE)
target_include_directories(plugin_interface INTERFACE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
set_target_properties(plugin_interface PROPERTIES PUBLIC_HEADER "${PLUGIN_INTERFACE_PUBLIC_HEADERES}")
target_link_libraries(plugin_interface INTERFACE std::filesystem nlohmann_json::nlohmann_json)

# Archive Lib
set(DOUKA_DEFAULT_PLUGIN_PATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/douka/plugin")
set(TARGET ${PROJECT_NAME}-static)
add_library(${TARGET} STATIC)
target_sources(${TARGET}
  PRIVATE
  ${CMAKE_SOURCE_DIR}/src/common/io.cc
  ${CMAKE_SOURCE_DIR}/src/filter/enkf.cc
  ${CMAKE_SOURCE_DIR}/src/filter/particle.cc
  ${CMAKE_SOURCE_DIR}/src/command/filter.cc
  ${CMAKE_SOURCE_DIR}/src/command/init.cc
  ${CMAKE_SOURCE_DIR}/src/command/predict.cc
  ${CMAKE_SOURCE_DIR}/src/command/obsgen.cc)
target_include_directories(${TARGET} PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(${TARGET}
  PUBLIC plugin_interface Eigen3::Eigen ${CMAKE_DL_LIBS}
  PRIVATE douka::mkl douka::blas)
target_compile_definitions(${TARGET} PRIVATE DOUKA_DEFAULT_PLUGIN_PATH="${DOUKA_DEFAULT_PLUGIN_PATH}")

configure_file(
  ${CMAKE_SOURCE_DIR}/cmake/template/douka_version.hh.in
  ${CMAKE_BINARY_DIR}/include/douka_version.hh)

# Main Executable
set(TARGET ${DOUKA_EXECUTABLE_NAME})
add_executable(${TARGET})
target_sources(${TARGET}
  PRIVATE ${CMAKE_SOURCE_DIR}/src/main.cc)
target_link_libraries(${TARGET} PRIVATE ${PROJECT_NAME}-static)
target_include_directories(${TARGET}
  PRIVATE
  ${PROJECT_BINARY_DIR}/include
  ${CMAKE_SOURCE_DIR}/src)

if(BUILD_TESTING)
  include(CTest)
  add_subdirectory(test)
endif()

if(BUILD_BENCHMARK)
  add_subdirectory(benchmark)
endif()

douka_install(${DOUKA_EXECUTABLE_NAME} plugin_interface)
douka_status(${PROJECT_NAME}-static)
