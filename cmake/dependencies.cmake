# Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
# SPDX-License-Identifier: Apache-2.0

find_package(Eigen3 REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)

add_library(douka::mkl INTERFACE IMPORTED)
if(DOUKA_USE_MKL)
  set(MKL_INTERFACE lp64)
  # Use gnu_thread for portability instead of intel_thread
  set(MKL_THREADING gnu_thread)
  set(MKL_LINK static)
  find_package(MKL CONFIG REQUIRED)
  set_property(
    TARGET douka::mkl PROPERTY
    INTERFACE_LINK_LIBRARIES MKL::MKL)
  set_property(
    TARGET douka::mkl PROPERTY
    INTERFACE_COMPILE_DEFINITIONS EIGEN_USE_MKL_ALL)
endif()

add_library(douka::blas INTERFACE IMPORTED)
if(DOUKA_USE_BLAS)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "FujitsuClang")
    set_property(
      TARGET douka::blas PROPERTY
      INTERFACE_LINK_LIBRARIES "-SSL2BLAMP -fopenmp")
  else()
    find_package(BLAS REQUIRED)
    find_package(LAPACKE REQUIRED)
    set_property(
      TARGET douka::blas PROPERTY
      INTERFACE_LINK_LIBRARIES BLAS::BLAS ${LAPACKE_LIBRARIES})
  endif()
  set_property(
    TARGET douka::blas PROPERTY
    INTERFACE_COMPILE_DEFINITIONS EIGEN_USE_BLAS EIGEN_USE_LAPACKE)
endif()
