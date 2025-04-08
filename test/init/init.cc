/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#include <command/init.hh>
#include <gtest/gtest.h>

void expect_ne(const std::vector<double> lhs, const std::vector<double> rhs) {
  ASSERT_EQ(lhs.size(), rhs.size());
  for (std::size_t i = 0; i < lhs.size(); ++i) {
    EXPECT_NE(lhs[i], rhs[i]);
  }
}

void expect_near(const std::vector<double> lhs, const std::vector<double> rhs,
                 const double epsilon = 1.0e-2) {
  ASSERT_EQ(lhs.size(), rhs.size());
  for (std::size_t i = 0; i < lhs.size(); ++i) {
    EXPECT_NEAR(lhs[i], rhs[i], epsilon);
  }
}

void expect_near(const std::vector<douka::io::State> states, const std::vector<double> rhs,
                 const double epsilon = 1.0e-2) {
  for (const auto &state : states) {
    expect_near(state.x, rhs, epsilon);
  }
}

TEST(init, validate_valid1) {
  douka::command::init::Param param = {"test", 0, 2, 3, {1.0, 2.0, 3.0}, {1.0, 1.0, 1.0}};

  ASSERT_TRUE(douka::command::init::validate(param));
}

TEST(init, validate_invalid1) {
  douka::command::init::Param param = {"test", 0, 2, 3, {1.0, 2.0}, {1.0, 1.0, 1.0}};

  ASSERT_FALSE(douka::command::init::validate(param));
}

TEST(init, validate_invalid2) {
  douka::command::init::Param param = {"test", 0, 2, 3, {1.0, 2.0, 3.0}, {1.0, 1.0}};

  ASSERT_FALSE(douka::command::init::validate(param));
}

TEST(init, validate_invalid3) {
  douka::command::init::Param param = {"", 0, 2, 3, {1.0, 2.0, 3.0}, {1.0, 1.0, 1.0}};

  ASSERT_FALSE(douka::command::init::validate(param));
}

TEST(init, validate) {
  const double sigma = 1.0e-2;
  douka::command::init::Param param = {
      "test",
      0,
      2,
      3,
      {1.0, 2.0, 3.0},
      {std::pow(sigma, 2), std::pow(sigma, 2), std::pow(sigma, 2)}};

  std::vector<douka::io::State> states;
  ASSERT_TRUE(douka::command::init::init(states, param));

  ASSERT_EQ(states.size(), param.N);
  expect_near(states, param.x0, 3 * sigma);
  expect_ne(states[0].x, states[1].x);
}