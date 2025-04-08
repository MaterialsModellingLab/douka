/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#include <filter/enkf.hh>
#include <gtest/gtest.h>

static void expect_states(const std::vector<douka::io::State> &states,
                          const std::vector<douka::io::State> &expect,
                          const double epsilon = 1e-2) {
  EXPECT_EQ(states.size(), expect.size());
  for (std::size_t i = 0; i < states.size(); ++i) {
    EXPECT_EQ(states[i].name, expect[i].name);
    EXPECT_EQ(states[i].id, expect[i].id);
    EXPECT_EQ(states[i].sys_tim, expect[i].sys_tim);
    EXPECT_EQ(states[i].obs_tim, expect[i].obs_tim);
    for (std::size_t j = 0; j < states[i].x.size(); ++j) {
      EXPECT_NEAR(states[i].x[j], expect[i].x[j], epsilon);
    }
  }
}

TEST(enkf, validate_valid1) {
  std::vector<douka::io::State> states = {
      {"test", 0, 1, 0, {1.0, 2.0, 3.0}},
      {"test", 1, 1, 0, {1.0, 2.0, 3.0}},
  };
  douka::io::Obs obs = {"test", 1, {1.0, 2.0, 3.0}};
  douka::filter::enkf::Param param = {
      "test", 0, 2, 3, 3, {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}, {}};

  ASSERT_TRUE(douka::filter::enkf::validate(states, obs, param));
}

TEST(enkf, validate_invalid1) {
  std::vector<douka::io::State> states = {
      {"tes", 0, 1, 0, {1.0, 2.0, 3.0}}, // invalid name
      {"test", 1, 1, 0, {1.0, 2.0, 3.0}},
  };
  douka::io::Obs obs = {"test", 1, {1.0, 2.0, 3.0}};
  douka::filter::enkf::Param param = {
      "test", 0, 2, 3, 3, {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}, {}};

  ASSERT_FALSE(douka::filter::enkf::validate(states, obs, param));
}

TEST(enkf, validate_invalid2) {
  std::vector<douka::io::State> states = {
      {"test", 0, 0, 0, {1.0, 2.0, 3.0}}, // invalid sys tim
      {"test", 1, 1, 0, {1.0, 2.0, 3.0}},
  };
  douka::io::Obs obs = {"test", 1, {1.0, 2.0, 3.0}};
  douka::filter::enkf::Param param = {
      "test", 0, 2, 3, 3, {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}, {}};

  ASSERT_FALSE(douka::filter::enkf::validate(states, obs, param));
}

TEST(enkf, filter1) {
  std::vector<douka::io::State> states = {
      {"test", 0, 1, 0, {1.0, 2.0, 3.0}},
      {"test", 1, 1, 0, {2.0, 4.0, 6.0}},
  };
  douka::io::Obs obs = {"test", 1, {1.5, 3.0, 4.5}};
  douka::filter::enkf::Param param = {"test", 0, 2, 3, 3, {1.0e+10, 1.0e+10, 1.0e+10}, {}};
  ASSERT_TRUE(douka::filter::enkf::filter(states, obs, param));

  std::vector<douka::io::State> expect = {
      {"test", 0, 1, 1, {1.0, 2.0, 3.0}},
      {"test", 1, 1, 1, {2.0, 4.0, 6.0}},
  };

  expect_states(states, expect);
}

TEST(enkf, filter2) {
  std::vector<douka::io::State> states = {
      {"test", 0, 1, 0, {1.0, 2.0, 3.0}},
      {"test", 1, 1, 0, {2.0, 4.0, 6.0}},
  };
  douka::io::Obs obs = {"test", 1, {1.5, 3.0}};
  douka::filter::enkf::Param param = {
      "test", 0, 2, 3, 2, {1.0e-10, 1.0e-10}, {1.0, 0.0, 0.0, 0.0, 1.0, 0.0}};

  ASSERT_TRUE(douka::filter::enkf::filter(states, obs, param));
  std::vector<douka::io::State> expect = {
      {"test", 0, 1, 1, {1.5, 3.0, 4.5}},
      {"test", 1, 1, 1, {1.5, 3.0, 4.5}},
  };

  expect_states(states, expect);
}