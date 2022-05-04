/*******************************************************************************
 * benchmarks/sdsl_sd_rank.hpp
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

#include <pasta/utils/do_not_optimize.hpp>
#include <pasta/utils/memory_monitor.hpp>
#include <pasta/utils/timer.hpp>
#include <sdsl/bit_vectors.hpp>

BenchmarkResult run_sdsl_sd_select(size_t const bit_size,
                                   size_t const fill_percentage,
                                   bool const is_adversarial,
                                   sdsl::bit_vector const& bv,
                                   std::vector<size_t> const& rank_positions,
                                   std::vector<size_t> const& select1_positions) {
  BenchmarkResult result;
  result.algo_name = "sdsl-sd-select";
  result.bit_size = bit_size;
  result.fill_percentage = fill_percentage;
  result.is_adversarial = is_adversarial;
  result.select1_query_count = select1_positions.size();

  typename sdsl::sd_vector<> sd_vec(bv);

  result.reported_compressed_bv_space = sdsl::size_in_bytes(sd_vec);
  
  pasta::Timer timer;
  pasta::MemoryMonitor& mem_monitor = pasta::MemoryMonitor::instance();

  timer.reset();
  mem_monitor.reset();

  typename sdsl::sd_vector<>::select_1_type select1_sd_bv(&sd_vec);

  result.rank_select_construction_time = timer.get_and_sleep_and_reset(5);
  auto const rs_mem_peak = mem_monitor.get_and_reset();
  result.rank_select_construction_memory_peak = rs_mem_peak.cur_peak;
  timer.reset();

  for (size_t i = 0; i < select1_positions.size(); ++i) {
    [[maybe_unused]] size_t const result = select1_sd_bv(select1_positions[i]);
    PASTA_DO_NOT_OPTIMIZE(result);
  }
  result.select1_query_time = timer.get_and_reset();

  auto const rs_query_mem_peak = mem_monitor.get_and_reset();
  result.rank_select_query_memory_peak = rs_query_mem_peak.cur_peak;

  return result;
}

/******************************************************************************/
