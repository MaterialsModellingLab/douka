/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#include <command/predict.hh>
#include <gtest/gtest.h>

class SamplePlugin : public douka::PluginInterface {
public:
  const double sigma = 1.0;
  bool predict(std::vector<double> &state, //
               const std::vector<double> &noise) override {
    EXPECT_DOUBLE_EQ(state.at(0), 1.0);
    EXPECT_DOUBLE_EQ(state.at(1), 2.0);
    EXPECT_DOUBLE_EQ(state.at(2), 3.0);

    // 99.7 % of random noise should be in the range of 3 sigma
    EXPECT_NEAR(noise.at(0), 0.0, 3.0 * this->sigma);
    EXPECT_NEAR(noise.at(1), 0.0, 3.0 * this->sigma);
    EXPECT_NEAR(noise.at(2), 0.0, 3.0 * this->sigma);

    return true;
  }
};

TEST(predict, validate_valid1) {
  douka::io::State state = {"test", 0, 0, 0, {1.0, 2.0, 3.0}};
  douka::command::predict::Param param = {
      "test", 0, 3, {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}};

  ASSERT_TRUE(douka::command::predict::validate(state, param));
}

TEST(predict, validate_invalid1) {
  douka::io::State state = {"tes", 0, 0, 0, {1.0, 2.0, 3.0}};
  douka::command::predict::Param param = {
      "test", 0, 3, {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}};

  ASSERT_FALSE(douka::command::predict::validate(state, param));
}

TEST(predict, validate_invalid2) {
  douka::io::State state = {"tes", 0, 0, 0, {1.0, 2.0}};
  douka::command::predict::Param param = {
      "test", 0, 3, {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}};

  ASSERT_FALSE(douka::command::predict::validate(state, param));
}

TEST(predict, validate_invalid3) {
  douka::io::State state = {"tes", 0, 0, 0, {1.0, 2.0}};
  douka::command::predict::Param param = {
      "test", 0, 4, {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}};

  ASSERT_FALSE(douka::command::predict::validate(state, param));
}

TEST(predict, predict1) {
  auto plugin = std::make_shared<SamplePlugin>();
  douka::io::State state = {"test", 0, 0, 0, {1.0, 2.0, 3.0}};
  douka::command::predict::Param param = {
      "test",
      0,
      3,
      {std::pow(plugin->sigma, 2.0), std::pow(plugin->sigma, 2.0), std::pow(plugin->sigma, 2.0)}};

  ASSERT_TRUE(douka::command::predict::predict(state, param, plugin));
}