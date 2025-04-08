/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __DOUKA_COMMON_COMPUTE__
#define __DOUKA_COMMON_COMPUTE__

#include <Eigen/Core>
#include <Eigen/QR>

#include <random>

namespace douka::common::compute {
template <typename Type, typename RandomEngine>
auto rand(const Eigen::VectorX<Type> &sigma, const Eigen::Index &N, RandomEngine &e) {
  static const auto random_functor = [&]() {
    static std::normal_distribution<> dist{0.0, 1.0};
    return dist(e);
  };
  const auto r = Eigen::MatrixX<Type>::NullaryExpr(sigma.rows(), N, random_functor);
  return (sigma.cwiseSqrt().asDiagonal() * r).eval();
}

template <typename Type, typename RandomEngine>
auto rand(const Eigen::MatrixX<Type> &sigma, const Eigen::Index &N, RandomEngine &e) {
  static const auto random_functor = [&]() {
    static std::normal_distribution<> dist{0.0, 1.0};
    return dist(e);
  };
  const auto r = Eigen::MatrixX<Type>::NullaryExpr(sigma.rows(), N, random_functor);
  return (sigma.llt().matrixL() * r).eval();
}

template <typename Arg1> auto mean_diff(const Eigen::MatrixBase<Arg1> &m) {
  return m.colwise() - m.rowwise().mean();
}

template <typename Arg1> auto cov(const Eigen::MatrixBase<Arg1> &m) {
  const auto md = mean_diff(m);
  return (1.0 / (m.cols() - 1.0)) * md * md.transpose();
}

template <typename Derive1, typename Derive2, typename Derive3>
auto kalman_gain(const Eigen::MatrixBase<Derive1> &X, const Eigen::MatrixBase<Derive2> &H,
                 const Eigen::MatrixBase<Derive3> &R) {
  const auto V = cov(X);
  return V * H.transpose() *
         (H * V * H.transpose() + R).completeOrthogonalDecomposition().pseudoInverse();
}

template <typename Derive1, typename Derive2, typename Derive3>
auto kalman_gain_tall(const Eigen::MatrixBase<Derive1> &X, const Eigen::MatrixBase<Derive2> &H,
                      const Eigen::MatrixBase<Derive3> &R) {
  const auto Z = (1.0 / std::sqrt(X.cols() - 1.0)) * mean_diff(X);
  const auto S = H * Z;
  return Z * S.transpose() *
         (S * S.transpose() + R).completeOrthogonalDecomposition().pseudoInverse();
}
} // namespace douka::common::compute
#endif