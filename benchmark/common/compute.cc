/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common/compute.hh"
#include <benchmark/benchmark.h>
#include <cstring>
#include <gperftools/malloc_hook.h>

benchmark::IterationCount g_num_new = 0;
benchmark::IterationCount g_sum_size_new = 0;
benchmark::IterationCount g_max_size_new = 0;
benchmark::IterationCount g_min_size_new = -1;
auto new_hook = [](const void *, std::size_t size) {
  ++g_num_new;
  g_sum_size_new += static_cast<benchmark::IterationCount>(size);
  g_max_size_new = std::max(g_max_size_new, static_cast<benchmark::IterationCount>(size));
  g_min_size_new = std::min(g_min_size_new, static_cast<benchmark::IterationCount>(size));
};

#define BEFORE_TEST                                                                                \
  benchmark::IterationCount num_new = g_num_new;                                                   \
  benchmark::IterationCount sum_size_new = g_sum_size_new;                                         \
  g_max_size_new = 0;                                                                              \
  g_min_size_new = -1;                                                                             \
  MallocHook::AddNewHook(new_hook);

#define AFTER_TEST                                                                                 \
  MallocHook::RemoveNewHook(new_hook);                                                             \
  auto iter = state.iterations();                                                                  \
  state.counters["#new"] = (g_num_new - num_new) / iter;                                           \
  state.counters["sum_new"] = (g_sum_size_new - sum_size_new) / iter;                              \
  state.counters["avg_new"] = (g_sum_size_new - sum_size_new) / (g_num_new - num_new);             \
  state.counters["max_new"] = g_max_size_new;                                                      \
  if (((benchmark::IterationCount) - 1) != g_min_size_new) {                                       \
    state.counters["min_new"] = g_min_size_new;                                                    \
  }

static const Eigen::Index N = 8;
static void BM_kalman_gain(benchmark::State &state) {
  BEFORE_TEST
  for (auto _ : state) {
    state.PauseTiming();
    const Eigen::Index k = state.range(0);
    Eigen::MatrixXd X_mat = Eigen::MatrixXd::Random(k, N);
    auto R_mat = Eigen::MatrixXd::Identity(k, k);
    auto H_mat = Eigen::MatrixXd::Identity(k, k);
    state.ResumeTiming();

    douka::common::compute::kalman_gain(X_mat, H_mat, R_mat).eval();
  }
  AFTER_TEST
}
BENCHMARK(BM_kalman_gain)->Iterations(10)->RangeMultiplier(16)->Range(2, 512);

static void BM_kalman_gain_tall(benchmark::State &state) {
  BEFORE_TEST
  for (auto _ : state) {
    state.PauseTiming();
    const Eigen::Index k = state.range(0);
    Eigen::MatrixXd X_mat = Eigen::MatrixXd::Random(k, N);
    auto R_mat = Eigen::MatrixXd::Identity(k, k);
    auto H_mat = Eigen::MatrixXd::Identity(k, k);
    state.ResumeTiming();

    douka::common::compute::kalman_gain_tall(X_mat, H_mat, R_mat).eval();
  }
  AFTER_TEST
}
BENCHMARK(BM_kalman_gain_tall)->Iterations(10)->RangeMultiplier(16)->Range(2, 512);

BENCHMARK_MAIN();
