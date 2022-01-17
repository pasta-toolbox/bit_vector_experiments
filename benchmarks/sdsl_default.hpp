/*******************************************************************************
 * benchmarks/sdsl_default.hpp
 *
 * Copyright (C) 2022 Florian Kurpicz <florian@kurpicz.org>
 *
 * pasta::bit_vector_experiments is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * pasta::bit_vector_experiments is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with pasta::bit_vector_experiments. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#pragma once

#include "benchmark_result.hpp"
#include "utils/do_not_optimize.hpp"
#include "utils/memory_monitor.hpp"
#include "utils/timer.hpp"

#include <random>
#include <sdsl/bit_vectors.hpp>
#include <tlx/math/aggregate.hpp>

BenchmarkResult run_sdsl_default(size_t const bit_size,
                                 size_t const fill_percentage,
                                 size_t const query_count,
                                 std::mt19937 randomness) {
  BenchmarkResult result;
  result.algo_name = "sdsl-defaul-algorithm";
  result.bit_size = bit_size;
  result.fill_percentage = fill_percentage;

  sdsl::bit_vector bv(bit_size, 0);
  std::uniform_int_distribution<> bit_dist(0, 99);
  for (size_t i = 0; i < bit_size; ++i) {
    bv[i] = (static_cast<uint32_t>(bit_dist(randomness)) < fill_percentage);
  }

  pasta::Timer timer;
  pasta::MemoryMonitor& mem_monitor = pasta::MemoryMonitor::instance();

  timer.reset();
  mem_monitor.reset();

  sdsl::bit_vector::select_0_type bvs0(&bv);
  sdsl::bit_vector::select_1_type bvs1(&bv);

  sdsl::bit_vector::rank_0_type bvr0(&bv);
  sdsl::bit_vector::rank_1_type bvr1(&bv);

  result.rank_select_construction_time = timer.get_and_reset();
  auto const rs_mem_peak = mem_monitor.get_and_reset();
  result.rank_select_construction_memory_peak = rs_mem_peak.cur_peak;
  timer.reset();

  std::uniform_int_distribution<> rank_dist(0, bit_size - 1);
  std::vector<size_t> rank_positions(query_count);

  tlx::Aggregate<size_t> rank_query_properties;
  for (auto& pos : rank_positions) {
    pos = rank_dist(randomness);
    rank_query_properties.add(pos);
  }

  size_t const zero_bits = bvr0.rank(bit_size);
  size_t const one_bits = bvr1.rank(bit_size);

  std::vector<size_t> select0_positions(query_count / 2);
  std::vector<size_t> select1_positions(query_count / 2 +
                                        ((query_count % 2 == 0) ? 0 : 1));
  std::uniform_int_distribution<> select0_dist(1, zero_bits);
  std::uniform_int_distribution<> select1_dist(1, one_bits);

  tlx::Aggregate<size_t> select0_query_properties;
  tlx::Aggregate<size_t> select1_query_properties;
  for (auto& pos : select0_positions) {
    pos = select0_dist(randomness);
    select0_query_properties.add(pos);
  }
  for (auto& pos : select1_positions) {
    pos = select1_dist(randomness);
    select1_query_properties.add(pos);
  }

  result.rank0_query_count = rank_positions.size() / 2;
  result.rank1_query_count = rank_positions.size() / 2;

  result.select0_query_count = select0_positions.size();
  result.select1_query_count = select1_positions.size();

  timer.reset();
  mem_monitor.reset();

  for (size_t i = 0; i < rank_positions.size() / 2; ++i) {
    [[maybe_unused]] size_t const result = bvr0.rank(rank_positions[i]);
    PASTA_DO_NOT_OPTIMIZE(result);
  }
  result.rank0_query_time = timer.get_and_reset();
  for (size_t i = rank_positions.size() / 2; i < rank_positions.size(); ++i) {
    [[maybe_unused]] size_t const result = bvr1.rank(rank_positions[i]);
    PASTA_DO_NOT_OPTIMIZE(result);
  }
  result.rank1_query_time = timer.get_and_reset();

  for (auto const pos : select0_positions) {
    [[maybe_unused]] size_t const result = bvs0.select(pos);
    PASTA_DO_NOT_OPTIMIZE(result);
  }
  result.select0_query_time = timer.get_and_reset();
  for (auto const pos : select1_positions) {
    [[maybe_unused]] size_t const result = bvs1.select(pos);
    PASTA_DO_NOT_OPTIMIZE(result);
  }
  result.select1_query_time = timer.get_and_reset();
  auto const rs_query_mem_peak = mem_monitor.get_and_reset();
  result.rank_select_query_memory_peak = rs_query_mem_peak.cur_peak;

  return result;
}

/******************************************************************************/
