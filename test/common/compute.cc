/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#include <common/compute.hh>
#include <gtest/gtest.h>

namespace compute = douka::common::compute;

TEST(common, compute_mean_diff) {
  Eigen::MatrixXd x_mat(3, 3);
  // clang-format off
  x_mat << 1.0, 2.0, 3.0,
           2.0, 4.0, 6.0,
           3.0, 6.0, 9.0;
  // clang-format on
  Eigen::MatrixXd expect_mat(3, 3);
  // clang-format off
  expect_mat << -1.0, 0.0, 1.0,
                -2.0, 0.0, 2.0,
                -3.0, 0.0, 3.0;
  // clang-format on
  const auto computed_mat = compute::mean_diff(x_mat);
  ASSERT_TRUE(computed_mat.isApprox(expect_mat));
}

TEST(common, compute_cov_mat1) {
  const Eigen::Index N = 2, k = 3;
  Eigen::MatrixXd x_mat(k, N);
  // clang-format off
  x_mat << 1.0, 1.1,
           2.0, 2.2,
           3.0, 3.3;
  // clang-format on
  Eigen::MatrixXd expect_mat(k, k);
  // clang-format off
  expect_mat << 5.0e-3, 1.0e-2, 1.5e-2,
                1.0e-2, 2.0e-2, 3.0e-2,
                1.5e-2, 3.0e-2, 4.5e-2;
  // clang-format on

  const auto compute_mat = compute::cov(x_mat);
  ASSERT_TRUE(compute_mat.isApprox(expect_mat));
}

TEST(common, compute_cov_from_rand1) {
  Eigen::MatrixXd cov_mat(3, 3);
  // clang-format off
  cov_mat << 1.0, 0.0, 0.0,
             0.0, 2.0, 0.0,
             0.0, 0.0, 3.0;
  // clang-format on
  std::default_random_engine engine{1};
  const auto rand = compute::rand(cov_mat, 100, engine);

  const auto computed_mat = compute::cov(rand);
  ASSERT_TRUE(computed_mat.isApprox(cov_mat, 0.3));
}

TEST(common, compute_cov_from_rand2) {
  Eigen::VectorXd cov_diag(3);
  cov_diag << 1.0, 2.0, 3.0;
  std::default_random_engine engine{1};

  const auto r = compute::rand(cov_diag, 100, engine);
  const auto computed_cov_mat = compute::cov(r);
  ASSERT_TRUE(computed_cov_mat.isApprox(cov_diag.asDiagonal().toDenseMatrix(), 0.3));
}

TEST(common, compute_kalman_gain) {
  const Eigen::Index N = 3, l = 5, k = 6;
  Eigen::MatrixXd X_mat(k, N);
  // clang-format off
  X_mat <<
    1.0, 2.0, 3.0,
    2.0, 4.0, 6.0,
    3.0, 6.0, 9.0,
    4.0, 8.0, 12.0,
    5.0, 10.0, 15.0,
    6.0, 12.0, 18.0;
  // clang-format on
  const auto R_mat = Eigen::MatrixXd::Identity(l, l) * 3.0;
  const auto H_mat = Eigen::MatrixXd::Identity(l, k);

  Eigen::MatrixXd K_expect_mat(k, l);
  // clang-format off
  K_expect_mat <<
    0.0172413793, 0.0344827586, 0.0517241379, 0.0689655172, 0.0862068966,
    0.0344827586, 0.0689655172, 0.1034482759, 0.1379310345, 0.1724137931,
    0.0517241379, 0.1034482759, 0.1551724138, 0.2068965517, 0.2586206897,
    0.0689655172, 0.1379310345, 0.2068965517, 0.2758620690, 0.3448275862,
    0.0862068966, 0.1724137931, 0.2586206897, 0.3448275862, 0.4310344828,
    0.1034482759, 0.2068965517, 0.3103448276, 0.4137931034, 0.5172413793;
  // clang-format on
  const auto K_mat = compute::kalman_gain(X_mat, H_mat, R_mat);
  ASSERT_TRUE(K_mat.isApprox(K_expect_mat, 1.0e-6));
}

TEST(common, compute_kalman_gain_tall) {
  const Eigen::Index N = 3, l = 5, k = 6;
  Eigen::MatrixXd X_mat(k, N);
  // clang-format off
  X_mat <<
    1.0, 2.0, 3.0,
    2.0, 4.0, 6.0,
    3.0, 6.0, 9.0,
    4.0, 8.0, 12.0,
    5.0, 10.0, 15.0,
    6.0, 12.0, 18.0;
  // clang-format on
  const auto R_mat = Eigen::MatrixXd::Identity(l, l) * 3.0;
  const auto H_mat = Eigen::MatrixXd::Identity(l, k);

  Eigen::MatrixXd K_expect_mat(k, l);
  // clang-format off
  K_expect_mat <<
    0.0172413793, 0.0344827586, 0.0517241379, 0.0689655172, 0.0862068966,
    0.0344827586, 0.0689655172, 0.1034482759, 0.1379310345, 0.1724137931,
    0.0517241379, 0.1034482759, 0.1551724138, 0.2068965517, 0.2586206897,
    0.0689655172, 0.1379310345, 0.2068965517, 0.2758620690, 0.3448275862,
    0.0862068966, 0.1724137931, 0.2586206897, 0.3448275862, 0.4310344828,
    0.1034482759, 0.2068965517, 0.3103448276, 0.4137931034, 0.5172413793;
  // clang-format on
  const auto K_mat = compute::kalman_gain_tall(X_mat, H_mat, R_mat);
  ASSERT_TRUE(K_mat.isApprox(K_expect_mat, 1.0e-6));
}
