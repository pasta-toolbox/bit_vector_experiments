/*******************************************************************************
 * benchmarks/rank_select_benchmark.cpp
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

#include <array>
#include <iostream>
#include <random>
#include <tlx/cmdline_parser.hpp>

#include "pasta_popcount.hpp"
#include "pasta_popcount_flat.hpp"
#include "poppy_rank_select.hpp"
#include "rank9_select.hpp"
#include "simple_select.hpp"
#include "simple_select_half.hpp"
#include "benchmark_result.hpp"
#include "sdsl_default.hpp"

// void run() {
//   sdsl::bit_vector bv(10000, 0);

//   sdsl::bit_vector::select_0_type bvs0(&bv);
//   sdsl::bit_vector::select_1_type bvs1(&bv);

//   sdsl::bit_vector::rank_0_type bvr0(&bv);
//   sdsl::bit_vector::rank_1_type bvr1(&bv);

//   std::array<uint64_t, 10000000/64> array;

//   sux::bits::Rank9Sel rs(array.data(), array.size() * 64);

//   BitmapPoppy bitmap(array.data(), array.size() * 64);

// }

class RankAndSelectBenchmark {
public:
  size_t bit_size_ = 1024 * 1024;
  uint32_t fill_percentage_ = 50;
  size_t query_count_ = 10'000;
  std::string filter_name_ = "";

  void run_benchmark_configuration() {
    std::random_device rd;
    std::mt19937 gen(rd());

    if (filter_name_.empty() || filter_name_ == "sdsl_default") {
      auto const result =
        run_sdsl_default(bit_size_, fill_percentage_, query_count_, gen);
      std::cout << result << "\n";
    }
    if (filter_name_.empty() || filter_name_ == "rank9_select") {
      auto const result =
        run_rank9_select(bit_size_, fill_percentage_, query_count_, gen);
      std::cout << result << "\n";
    }
    if (filter_name_.empty() || filter_name_ == "simple_select") {
      {
        auto const result =
          run_simple_select<0>(bit_size_, fill_percentage_, query_count_, gen);
        std::cout << result << "\n";
      }
      {
        auto const result =
          run_simple_select<1>(bit_size_, fill_percentage_, query_count_, gen);
        std::cout << result << "\n";
      }
      {
        auto const result =
          run_simple_select<2>(bit_size_, fill_percentage_, query_count_, gen);
        std::cout << result << "\n";
      }
      {
        auto const result =
          run_simple_select<3>(bit_size_, fill_percentage_, query_count_, gen);
        std::cout << result << "\n";
      }
    }
    if (filter_name_.empty() || filter_name_ == "simple_select_half") {
      auto const result =
        run_simple_select_half(bit_size_, fill_percentage_, query_count_, gen);
      std::cout << result << "\n";
    }
    // if (filter_name_.empty() || filter_name_ == "poppy_rank_select") {
    //   auto const result =
    //     run_poppy_rank_select(bit_size_, fill_percentage_, query_count_, gen);
    //   std::cout << result << "\n";
    // }
    if (filter_name_.empty() || filter_name_ == "pasta_popcount") {
      auto const result =
        run_pasta_popcount(bit_size_, fill_percentage_, query_count_, gen);
      std::cout << result << "\n";
    }
    if (filter_name_.empty() || filter_name_ == "pasta_popcount_flat") {
      auto const result =
        run_pasta_popcount_flat(bit_size_, fill_percentage_, query_count_, gen);
      std::cout << result << "\n";
    }
  }
};

int32_t main(int32_t argc, char const* const argv[]) {
  RankAndSelectBenchmark rsb;
  tlx::CmdlineParser cp;

  cp.set_description("Benchmark tool for PaStA's rank and select data "
                     "structures and other rank and select implementations.");
  cp.set_author("Florian Kurpicz <florian@kurpicz.org>");

  cp.add_bytes('b',
               "bit_size",
               rsb.bit_size_,
               "Size of the bit vector in bits "
               "(accepts SI units, default 1024^2.");

  cp.add_uint('f',
              "fill_percentage",
              rsb.fill_percentage_,
              "Percentage of set "
              "bits in the bit vector (default 50%).");

  cp.add_bytes('q',
               "query_count",
               rsb.query_count_,
               "Number of rank and select"
               " queries (accepts SI units, default is 10000)");

  cp.add_string('n', "name", rsb.filter_name_, "Runs the named algorithm.");

  if (!cp.process(argc, argv)) {
    return -1;
  }

  rsb.run_benchmark_configuration();

  return 0;
}

/******************************************************************************/
