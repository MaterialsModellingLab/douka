# Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
# SPDX-License-Identifier: Apache-2.0

find_package(benchmark REQUIRED)

function(add_gbench_target COMMAND_NAME TEST_NAME)
  set(TARGET "bench-${COMMAND_NAME}-${TEST_NAME}")
  add_executable(${TARGET})
  target_sources(${TARGET}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/${COMMAND_NAME}/${TEST_NAME}.cc)
  target_include_directories(${TARGET}
    PRIVATE
    ${GTEST_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/src)
  target_link_libraries(${TARGET} ${PROJECT_NAME}-static benchmark::benchmark tcmalloc)
endfunction()

add_gbench_target("common" "compute")
