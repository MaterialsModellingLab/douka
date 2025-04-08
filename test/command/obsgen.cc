/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#include <command/obsgen.hh>
#include <gtest/gtest.h>

namespace obsgen = douka::command::obsgen;

TEST(command_obsgen, show_help) {
  const char *argv[] = {"douka", "obsgen", "--help"};
  const int argc = sizeof(argv) / sizeof(char *);
  obsgen::Args args;
  ASSERT_THROW(args = obsgen::get_args(argc, argv), std::invalid_argument);
}

TEST(command_obsgen, missing_requirements1) {
  const char *argv[] = {"douka", "obsgen"};
  const int argc = sizeof(argv) / sizeof(char *);
  obsgen::Args args;
  ASSERT_THROW(args = obsgen::get_args(argc, argv), std::invalid_argument);
}

TEST(command_obsgen, ok1) {
  const char *argv[] = {"douka",         "obsgen",   "--param", "param1",
                        "param2",        "--plugin", "plugin1", "--plugin_param",
                        "plugin_param1", "--output", "out",     "--force"};
  const int argc = sizeof(argv) / sizeof(char *);

  obsgen::Args args;
  ASSERT_NO_THROW(args = obsgen::get_args(argc, argv));

  ASSERT_EQ(args.param.size(), 2);
  ASSERT_EQ(args.param.at(0), "param1");
  ASSERT_EQ(args.param.at(1), "param2");
  ASSERT_EQ(args.plugin, "plugin1");
  ASSERT_EQ(args.plugin_param, "plugin_param1");
  ASSERT_EQ(args.output, "out");
  ASSERT_TRUE(args.force);
}