# Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
# SPDX-License-Identifier: Apache-2.0

function(build_doc)
  find_package(Doxygen REQUIRED)
  find_program(SPHINX_BIN NAMES sphinx-build)
  if (NOT SPHINX_BIN)
    message(FATAL_ERROR "Sphinx not found. Please install Sphinx.")
    return()
  endif()

  configure_file(${CMAKE_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_BINARY_DIR}/Doxyfile.out @ONLY)
  doxygen_add_docs(
    doc-doxygen
    COMMENT "Generate doxygen documentation"
    CONFIG_FILE ${CMAKE_BINARY_DIR}/Doxyfile.out)

  set(SPHINX_SOURCE ${CMAKE_SOURCE_DIR}/doc/source)
  set(SPHINX_BUILD ${CMAKE_BINARY_DIR})
  add_custom_target(
    doc-html
    COMMAND ${SPHINX_BIN} -M html ${SPHINX_SOURCE} ${SPHINX_BUILD} -E -D version=${PROJECT_VERSION}
    DEPENDS doc-doxygen
  )

  add_custom_target(
    doc-pdf
    COMMAND ${SPHINX_BIN} -M latex ${SPHINX_SOURCE} ${SPHINX_BUILD} -E -D version=${PROJECT_VERSION}
    COMMAND make -C ${SPHINX_BUILD}/latex
    DEPENDS doc-doxygen
  )

  add_custom_target(doc ALL DEPENDS doc-html)
endfunction(build_doc)
