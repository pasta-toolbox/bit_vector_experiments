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

#include <sux/bits/SimpleSelectHalf.hpp>

BenchmarkResult run_simple_select_half(size_t const bit_size,
                                       size_t const fill_percentage,
                                       pasta::BitVector const& bv,
                                       std::vector<size_t> const& select1_positions) {
  BenchmarkResult result;
  result.algo_name = "sux-SimpleSelectHalf";
  result.bit_size = bit_size;
  result.fill_percentage = fill_percentage;
  result.rank1_query_count = 0;
  result.select1_query_count = select1_positions.size();
  
  auto const bv_data = bv.data();
  pasta::Timer timer;
  pasta::MemoryMonitor& mem_monitor = pasta::MemoryMonitor::instance();

  timer.reset();
  mem_monitor.reset();

  sux::bits::SimpleSelectHalf rs(bv_data.data(), bv_data.size() * 64);

  result.rank_select_construction_time = timer.get_and_reset();
  auto const rs_mem_peak = mem_monitor.get_and_reset();
  result.rank_select_construction_memory_peak = rs_mem_peak.cur_peak;
  timer.reset();

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
