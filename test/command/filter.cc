/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#include <command/filter.hh>
#include <gtest/gtest.h>

namespace filter = douka::command::filter;

TEST(command_filter, show_help) {
  const char *argv[] = {"douka", "filter", "--help"};
  const int argc = sizeof(argv) / sizeof(char *);
  filter::Args args;
  ASSERT_THROW(args = filter::get_args(argc, argv), std::invalid_argument);
}

TEST(command_filter, missing_requirements1) {
  const char *argv[] = {"douka", "filter"};
  const int argc = sizeof(argv) / sizeof(char *);
  filter::Args args;
  ASSERT_THROW(args = filter::get_args(argc, argv), std::invalid_argument);
}

TEST(command_filter, ok1) {
  const char *argv[] = {"douka", "filter", "--state",  "state1", "--param",  "param1", "param2",
                        "--obs", "obs1",   "--filter", "enkf",   "--output", "out",    "--force"};
  const int argc = sizeof(argv) / sizeof(char *);
  filter::Args args;
  ASSERT_NO_THROW(args = filter::get_args(argc, argv));

  ASSERT_EQ(args.state, "state1");
  ASSERT_EQ(args.param.size(), 2);
  ASSERT_EQ(args.param.at(0), "param1");
  ASSERT_EQ(args.param.at(1), "param2");
  ASSERT_EQ(args.obs, "obs1");
  ASSERT_EQ(args.filter, "enkf");
  ASSERT_EQ(args.output, "out");
  ASSERT_TRUE(args.force);
}