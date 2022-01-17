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
#include "bit_vector/bit_vector.hpp"
#include "utils/do_not_optimize.hpp"
#include "utils/memory_monitor.hpp"
#include "utils/timer.hpp"

#include <random>
#include <sux/bits/SimpleSelect.hpp>
#include <tlx/math/aggregate.hpp>

template <size_t Tuning>
BenchmarkResult run_simple_select(size_t const bit_size,
                                 size_t const fill_percentage,
                                 size_t const query_count,
                                 std::mt19937 randomness) {
  BenchmarkResult result;
  result.algo_name = "sux-SimpleSelect-" + std::to_string(Tuning);
  result.bit_size = bit_size;
  result.fill_percentage = fill_percentage;
  
  pasta::BitVector bv(bit_size, 0);
  std::uniform_int_distribution<> bit_dist(0, 99);
  for (size_t i = 0; i < bit_size; ++i) {
    bv[i] = (static_cast<uint32_t>(bit_dist(randomness)) < fill_percentage);
  }
  auto const bv_data = bv.data();
  pasta::Timer timer;
  pasta::MemoryMonitor& mem_monitor = pasta::MemoryMonitor::instance();

  timer.reset();
  mem_monitor.reset();

  sux::bits::SimpleSelect rs(bv_data.data(), bv_data.size() * 64, Tuning);

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

  sux::bits::Rank9 rank(bv_data.data(), bv_data.size() * 64);
  size_t const one_bits = rank.rank(bit_size);

  std::vector<size_t> select1_positions(query_count);
  std::uniform_int_distribution<> select1_dist(1, one_bits);

  tlx::Aggregate<size_t> select1_query_properties;
  for (auto& pos : select1_positions) {
    pos = select1_dist(randomness);
    select1_query_properties.add(pos);
  }

  result.rank1_query_count = 0;
  result.select1_query_count = select1_positions.size();

  timer.reset();
  mem_monitor.reset();

  for (auto const pos : select1_positions) {
    [[maybe_unused]] size_t const result = rs.select(pos);
    PASTA_DO_NOT_OPTIMIZE(result);
  }
  result.select1_query_time = timer.get_and_reset();
  auto const rs_query_mem_peak = mem_monitor.get_and_reset();
  result.rank_select_query_memory_peak = rs_query_mem_peak.cur_peak;

  return result;
}

/******************************************************************************/
