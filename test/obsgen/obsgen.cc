/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#include <command/obsgen.hh>
#include <gtest/gtest.h>

class SamplePlugin : public douka::PluginInterface {
public:
  bool predict(std::vector<double> &state, //
               const std::vector<double> &noise) override {
    for (const auto &n : noise) {
      EXPECT_DOUBLE_EQ(n, 0.0);
    }

    for (auto &s : state) {
      s = s + 1.0;
    }
    return true;
  }
};

TEST(obsgen, validate_valid1) {
  douka::command::obsgen::Param param = {
      "test", 0, 10, 3, 2, {1.0, 2.0, 3.0}, {1, 0, 0, 1, 0, 0}};

  ASSERT_TRUE(douka::command::obsgen::validate(param));
}

TEST(obsgen, validate_invalid1) {
  douka::command::obsgen::Param param = {
      "test", 0, 10, 2, 2, {1.0, 2.0, 3.0}, {1, 0, 0, 1, 0, 0}};

  ASSERT_FALSE(douka::command::obsgen::validate(param));
}

TEST(obsgen, validate_invalid2) {
  douka::command::obsgen::Param param = {
      "test", 0, 10, 3, 1, {1.0, 2.0, 3.0}, {1, 0, 0, 1, 0, 0}};

  ASSERT_FALSE(douka::command::obsgen::validate(param));
}

TEST(obsgen, validate_invalid3) {
  douka::command::obsgen::Param param = {
      "", 0, 10, 3, 2, {1.0, 2.0, 3.0}, {1, 0, 0, 1, 0, 0}};

  ASSERT_FALSE(douka::command::obsgen::validate(param));
}

TEST(obsgen, validate_invalid4) {
  douka::command::obsgen::Param param = {
      "test", 0, 0, 3, 1, {1.0, 2.0, 3.0}, {1, 0, 0, 1, 0, 0}};

  ASSERT_FALSE(douka::command::obsgen::validate(param));
}

TEST(obsgen, validate_invalid5) {
  douka::command::obsgen::Param param = {
      "test", 0, 10, 0, 1, {1.0, 2.0, 3.0}, {1, 0, 0, 1, 0, 0}};

  ASSERT_FALSE(douka::command::obsgen::validate(param));
}

TEST(obsgen, validate_invalid6) {
  douka::command::obsgen::Param param = {
      "test", 0, 10, 3, 0, {1.0, 2.0, 3.0}, {1, 0, 0, 1, 0, 0}};

  ASSERT_FALSE(douka::command::obsgen::validate(param));
}

TEST(obsgen, obsgen) {
  auto plugin = std::make_unique<SamplePlugin>();

  const double sigma = 0.1;
  douka::command::obsgen::Param param = {
      "test", 0, 10, 3, 2, {1.0, 2.0, 3.0}, {}};

  std::vector<douka::io::Obs> observations;
  ASSERT_TRUE(douka::command::obsgen::obsgen(observations, param, std::move(plugin)));
  ASSERT_EQ(observations.size(), param.t + 1);

  double y1 = param.x0[0];
  double y2 = param.x0[1];
  int64_t t = 0;
  for (const auto &obs : observations) {
    std::cout << obs << std::endl;

    ASSERT_EQ(obs.name, param.name);
    ASSERT_EQ(obs.obs_tim, t);

    ASSERT_EQ(obs.y.size(), param.l);
    EXPECT_NEAR(obs.y.at(0), y1, 3.0 * sigma);
    EXPECT_NEAR(obs.y.at(1), y2, 3.0 * sigma);

    y1 += 1.0;
    y2 += 1.0;
    t++;
  }
}