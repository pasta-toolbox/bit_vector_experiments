/*******************************************************************************
 * benchmark_result.hpp
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

#include <ostream>

struct BenchmarkResult {
  std::string algo_name = "";
  size_t bit_size = 0;
  size_t fill_percentage = 0;
  size_t rank_select_construction_time = 0;
  size_t rank_select_construction_memory_peak = 0;
  size_t rank1_query_count = 0;
  size_t rank1_query_time = 0;
  size_t select0_query_count = 0;
  size_t select0_query_time = 0;
  size_t select1_query_count = 0;
  size_t select1_query_time = 0;
  size_t rank_select_query_memory_peak = 0;
};

std::ostream& operator<<(std::ostream& os, BenchmarkResult const& br) {
  os << "RESULT"
     << " algo=" << br.algo_name << " bit_size=" << br.bit_size
     << " fill_percentage=" << br.fill_percentage
     << " rank_select_construction_time=" << br.rank_select_construction_time
     << " rank_select_memory_peak=" << br.rank_select_construction_memory_peak
     << " rank1_query_count=" << br.rank1_query_count
     << " rank1_query_time=" << br.rank1_query_time
     << " select0_query_count=" << br.select0_query_count
     << " select0_query_time=" << br.select0_query_time
     << " select1_query_count=" << br.select1_query_count
     << " select1_query_time=" << br.select1_query_time
     << " rank_select_query_memory_peak=" << br.rank_select_query_memory_peak;
  return os;
}

/******************************************************************************/
