/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#include <command/predict.hh>
#include <gtest/gtest.h>

namespace predict = douka::command::predict;

TEST(command_predict, show_help) {
  const char *argv[] = {"douka", "predict", "--help"};
  const int argc = sizeof(argv) / sizeof(char *);
  predict::Args args;
  ASSERT_THROW(args = predict::get_args(argc, argv), std::invalid_argument);
}

TEST(command_predict, missing_requirements1) {
  const char *argv[] = {"douka", "predict"};
  const int argc = sizeof(argv) / sizeof(char *);
  predict::Args args;
  ASSERT_THROW(args = predict::get_args(argc, argv), std::invalid_argument);
}

TEST(command_predict, ok1) {
  const char *argv[] = {"douka",         "predict",  "--state",  "state1",  "--param",
                        "param1",        "param2",   "--plugin", "plugin1", "--plugin_param",
                        "plugin_param1", "--output", "out1",     "--force"};
  const int argc = sizeof(argv) / sizeof(char *);
  predict::Args args;
  ASSERT_NO_THROW(args = predict::get_args(argc, argv));

  ASSERT_EQ(args.state, "state1");
  ASSERT_EQ(args.param.size(), 2);
  ASSERT_EQ(args.param.at(0), "param1");
  ASSERT_EQ(args.param.at(1), "param2");
  ASSERT_EQ(args.plugin, "plugin1");
  ASSERT_EQ(args.plugin_param, "plugin_param1");
  ASSERT_EQ(args.output, "out1");
  ASSERT_TRUE(args.force);
}