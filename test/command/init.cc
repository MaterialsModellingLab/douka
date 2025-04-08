/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#include <command/init.hh>
#include <gtest/gtest.h>

namespace init = douka::command::init;

TEST(command_init, show_help) {
  const char *argv[] = {"douka", "init", "--help"};
  const int argc = sizeof(argv) / sizeof(char *);
  init::Args args;
  ASSERT_THROW(args = init::get_args(argc, argv), std::invalid_argument);
}

TEST(command_init, missing_requirements1) {
  const char *argv[] = {"douka", "init"};
  const int argc = sizeof(argv) / sizeof(char *);
  init::Args args;
  ASSERT_THROW(args = init::get_args(argc, argv), std::invalid_argument);
}

TEST(command_init, ok1) {
  const char *argv[] = {"douka",  "init",    "--param",  "param1",
                        "param2", "--force", "--output", "out"};
  const int argc = sizeof(argv) / sizeof(char *);
  init::Args args;
  ASSERT_NO_THROW(args = init::get_args(argc, argv));

  ASSERT_EQ(args.param.size(), 2);
  ASSERT_EQ(args.param.at(0), "param1");
  ASSERT_EQ(args.param.at(1), "param2");
  ASSERT_EQ(args.output, "out");
  ASSERT_TRUE(args.force);
}