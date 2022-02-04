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

#include "benchmark_result.hpp"
#include "pasta_popcount.hpp"
#include "pasta_popcount_flat.hpp"
#include "pasta_popcount_wide.hpp"
#include "poppy_rank_select.hpp"
#include "poppy_rank_select_improved.hpp"
#include "rank9_select.hpp"
#include "sdsl_default_rank.hpp"
#include "sdsl_default_select.hpp"
#include "sdsl_rank_v5.hpp"
#include "simple_select.hpp"
#include "simple_select_half.hpp"

#include <array>
#include <chrono>
#include <iostream>
#include <ostream>
#include <random>
#include <thread>
#include <tlx/cmdline_parser.hpp>
#include <tlx/math/aggregate.hpp>

class RankAndSelectBenchmark {
public:
  size_t bit_size_ = 1024 * 1024;
  bool adversarial_distribution_ = false;
  uint32_t fill_percentage_ = 50;
  size_t query_count_ = 10'000;
  std::string filter_name_ = "";

  void run_benchmark_configuration() const {
    // Prepare benchmark setup (just once, to speed everything up
    std::random_device rd;
    std::mt19937 gen(rd());

    // Special setup for sdsl rank and select data structures, as they
    // require a different bit vector.
    if (filter_name_.empty() ||
        (filter_name_.find("sdsl") != std::string::npos)) {
      run_sdsl_benchmarks(gen);
    }
    if (filter_name_.empty() ||
        (filter_name_.find("sdsl") == std::string::npos)) {
      run_non_sdsl_benchmarks(gen);
    }
  }

private:
  void run_non_sdsl_benchmarks(std::mt19937 randomness) const {
    using namespace std::chrono_literals;

    size_t one_bits = 0;
    pasta::BitVector bv(bit_size_, 0);
    std::uniform_int_distribution<size_t> bit_dist(0, 99);
    if (!adversarial_distribution_) {
      for (size_t i = 0; i < bit_size_; ++i) {
        bool const flip_bit =
            (static_cast<uint32_t>(bit_dist(randomness)) < fill_percentage_);
        one_bits += flip_bit ? 1 : 0;
        bv[i] = flip_bit;
      }
    } else {
      for (size_t i = 0; i < (bit_size_ / 100) * (100 - fill_percentage_);
           ++i) {
        bool const flip_bit = (static_cast<uint32_t>(bit_dist(randomness)) < 1);
        one_bits += flip_bit ? 1 : 0;
        bv[i] = flip_bit;
      }
      for (size_t i = (bit_size_ / 100) * (100 - fill_percentage_);
           i < bit_size_;
           ++i) {
        bool const flip_bit =
            (static_cast<uint32_t>(bit_dist(randomness)) < 99);
        one_bits += flip_bit ? 1 : 0;
        bv[i] = flip_bit;
      }
    }
    size_t const zero_bits = bit_size_ - one_bits;

    std::cout << "Finished flipping bits in bit vector (" << one_bits
              << " bits flipped)" << std::endl;

    std::uniform_int_distribution<size_t> rank_dist(0, bit_size_ - 1);
    std::vector<size_t> rank_positions(query_count_);

    tlx::Aggregate<size_t> rank_query_properties;
    for (auto& pos : rank_positions) {
      pos = rank_dist(randomness);
      rank_query_properties.add(pos);
    }

    std::cout << "Finished creating rank query positions (min pos: "
              << rank_query_properties.min()
              << ", max pos: " << rank_query_properties.max()
              << ", avg pos:" << rank_query_properties.avg() << ")"
              << std::endl;

    std::vector<size_t> select0_positions(query_count_);
    std::uniform_int_distribution<size_t> select0_dist(1, zero_bits - 1);

    tlx::Aggregate<size_t> select0_query_properties;
    for (auto& pos : select0_positions) {
      pos = select0_dist(randomness);
      select0_query_properties.add(pos);
    }

    std::cout << "Finished creating select0 query ranks (min rank: "
              << select0_query_properties.min()
              << ", max rank: " << select0_query_properties.max()
              << ", avg rank:" << select0_query_properties.avg() << ")"
              << std::endl;

    std::vector<size_t> select1_positions(query_count_);
    std::uniform_int_distribution<size_t> select1_dist(1, one_bits - 1);

    tlx::Aggregate<size_t> select1_query_properties;
    for (auto& pos : select1_positions) {
      pos = select1_dist(randomness);
      select1_query_properties.add(pos);
    }

    std::cout << "Finished creating select1 query ranks (min rank: "
              << select1_query_properties.min()
              << ", max rank: " << select1_query_properties.max()
              << ", avg rank:" << select1_query_properties.avg() << ")"
              << std::endl;
    std::this_thread::sleep_for(5s);

    if (filter_name_.empty() || filter_name_ == "rank9_select") {
      auto const result = run_rank9_select(bit_size_,
                                           fill_percentage_,
                                           adversarial_distribution_,
                                           bv,
                                           rank_positions,
                                           select1_positions);
      std::cout << result << std::endl;
    }
    if (filter_name_.empty() || filter_name_ == "simple_select") {
      {
        auto const result = run_simple_select<0>(bit_size_,
                                                 fill_percentage_,
                                                 adversarial_distribution_,
                                                 bv,
                                                 select1_positions);
        std::cout << result << std::endl;
        std::this_thread::sleep_for(5s);
      }
      {
        auto const result = run_simple_select<1>(bit_size_,
                                                 fill_percentage_,
                                                 adversarial_distribution_,
                                                 bv,
                                                 select1_positions);
        std::cout << result << std::endl;
        std::this_thread::sleep_for(5s);
      }
      {
        auto const result = run_simple_select<2>(bit_size_,
                                                 fill_percentage_,
                                                 adversarial_distribution_,
                                                 bv,
                                                 select1_positions);
        std::cout << result << std::endl;
        std::this_thread::sleep_for(5s);
      }
      {
        auto const result = run_simple_select<3>(bit_size_,
                                                 fill_percentage_,
                                                 adversarial_distribution_,
                                                 bv,
                                                 select1_positions);
        std::cout << result << std::endl;
        std::this_thread::sleep_for(5s);
      }
    }
    if (filter_name_.empty() || filter_name_ == "simple_select_half") {
      auto const result = run_simple_select_half(bit_size_,
                                                 fill_percentage_,
                                                 adversarial_distribution_,
                                                 bv,
                                                 select1_positions);
      std::cout << result << std::endl;
      std::this_thread::sleep_for(5s);
    }
    if (filter_name_.empty() || filter_name_ == "poppy_rank_select") {
      auto const result = run_poppy_rank_select(bit_size_,
                                                fill_percentage_,
                                                adversarial_distribution_,
                                                bv,
                                                rank_positions,
                                                select1_positions);
      std::cout << result << std::endl;
      std::this_thread::sleep_for(5s);
    }
    if (filter_name_.empty() || filter_name_ == "poppy_rank_select_improved") {
      auto const result =
          run_poppy_rank_select_improved(bit_size_,
                                         fill_percentage_,
                                         adversarial_distribution_,
                                         bv,
                                         rank_positions,
                                         select1_positions);
      std::cout << result << std::endl;
      std::this_thread::sleep_for(5s);
    }
    if (filter_name_.empty() || filter_name_ == "pasta_popcount") {
      auto const result = run_pasta_popcount(bit_size_,
                                             fill_percentage_,
                                             adversarial_distribution_,
                                             bv,
                                             rank_positions,
                                             select0_positions,
                                             select1_positions);
      std::cout << result << std::endl;
      std::this_thread::sleep_for(5s);
    }
    if (filter_name_.empty() || filter_name_ == "pasta_popcount_flat") {
      {
        auto const result =
            run_pasta_popcount_flat<pasta::OptimizedFor::DONT_CARE,
                                    pasta::FindL2FlatWith::LINEAR_SEARCH>(
                bit_size_,
                fill_percentage_,
                adversarial_distribution_,
                bv,
                rank_positions,
                select0_positions,
                select1_positions);
        std::cout << result << std::endl;
        std::this_thread::sleep_for(5s);
      }
      {
        auto const result =
            run_pasta_popcount_flat<pasta::OptimizedFor::DONT_CARE,
                                    pasta::FindL2FlatWith::BINARY_SEARCH>(
                bit_size_,
                fill_percentage_,
                adversarial_distribution_,
                bv,
                rank_positions,
                select0_positions,
                select1_positions);
        std::cout << result << std::endl;
        std::this_thread::sleep_for(5s);
      }
      {
        auto const result =
            run_pasta_popcount_flat<pasta::OptimizedFor::DONT_CARE,
                                    pasta::FindL2FlatWith::INTRINSICS>(
                bit_size_,
                fill_percentage_,
                adversarial_distribution_,
                bv,
                rank_positions,
                select0_positions,
                select1_positions);
        std::cout << result << std::endl;
        std::this_thread::sleep_for(5s);
      }
    }
    if (filter_name_.empty() || filter_name_ == "pasta_popcount_wide") {
      {
        auto const result =
            run_pasta_popcount_wide<pasta::OptimizedFor::DONT_CARE,
                                    pasta::FindL2WideWith::LINEAR_SEARCH>(
                bit_size_,
                fill_percentage_,
                adversarial_distribution_,
                bv,
                rank_positions);
        std::cout << result << std::endl;
        std::this_thread::sleep_for(5s);
      }
      {
        auto const result =
            run_pasta_popcount_wide<pasta::OptimizedFor::DONT_CARE,
                                    pasta::FindL2WideWith::BINARY_SEARCH>(
                bit_size_,
                fill_percentage_,
                adversarial_distribution_,
                bv,
                rank_positions);
        std::cout << result << std::endl;
        std::this_thread::sleep_for(5s);
      }
    }
  }

  void run_sdsl_benchmarks(std::mt19937 randomness) const {
    using namespace std::chrono_literals;

    size_t one_bits = 0;
    sdsl::bit_vector bv(bit_size_, 0);
    std::uniform_int_distribution<size_t> bit_dist(0, 99);
    if (!adversarial_distribution_) {
      for (size_t i = 0; i < bit_size_; ++i) {
        bool const flip_bit =
            (static_cast<uint32_t>(bit_dist(randomness)) < fill_percentage_);
        one_bits += flip_bit ? 1 : 0;
        bv[i] = flip_bit;
      }
    } else {
      for (size_t i = 0; i < (bit_size_ / 100) * (100 - fill_percentage_);
           ++i) {
        bool const flip_bit = (static_cast<uint32_t>(bit_dist(randomness)) < 1);
        one_bits += flip_bit ? 1 : 0;
        bv[i] = flip_bit;
      }
      for (size_t i = (bit_size_ / 100) * (100 - fill_percentage_);
           i < bit_size_;
           ++i) {
        bool const flip_bit =
            (static_cast<uint32_t>(bit_dist(randomness)) < 99);
        one_bits += flip_bit ? 1 : 0;
        bv[i] = flip_bit;
      }
    }

    std::cout << "Finished flipping bits in bit vector (" << one_bits
              << ") bits flipped" << std::endl;

    std::uniform_int_distribution<size_t> rank_dist(0, bit_size_ - 1);
    std::vector<size_t> rank_positions(query_count_);

    tlx::Aggregate<size_t> rank_query_properties;
    for (auto& pos : rank_positions) {
      pos = rank_dist(randomness);
      rank_query_properties.add(pos);
    }

    std::cout << "Finished creating rank query positions (min pos: "
              << rank_query_properties.min()
              << ", max pos: " << rank_query_properties.max()
              << ", avg pos:" << rank_query_properties.avg() << ")"
              << std::endl;

    std::vector<size_t> select1_positions(query_count_);
    std::uniform_int_distribution<size_t> select1_dist(1, one_bits);

    tlx::Aggregate<size_t> select1_query_properties;
    for (auto& pos : select1_positions) {
      pos = select1_dist(randomness);
      select1_query_properties.add(pos);
    }

    std::cout << "Finished creating select1 query ranks (min rank: "
              << select1_query_properties.min()
              << ", max rank: " << select1_query_properties.max()
              << ", avg rank:" << select1_query_properties.avg() << ")"
              << std::endl;

    std::this_thread::sleep_for(5s);

    if (filter_name_.empty() || filter_name_ == "sdsl_default_rank") {
      auto const result = run_sdsl_default_rank(bit_size_,
                                                fill_percentage_,
                                                adversarial_distribution_,
                                                bv,
                                                rank_positions,
                                                select1_positions);
      std::cout << result << std::endl;
      std::this_thread::sleep_for(5s);
    }
    if (filter_name_.empty() || filter_name_ == "sdsl_default_select") {
      auto const result = run_sdsl_default_select(bit_size_,
                                                  fill_percentage_,
                                                  adversarial_distribution_,
                                                  bv,
                                                  rank_positions,
                                                  select1_positions);
      std::cout << result << std::endl;
      std::this_thread::sleep_for(5s);
    }
    if (filter_name_.empty() || filter_name_ == "sdsl_rank_v5") {
      auto const result = run_sdsl_rank_v5(bit_size_,
                                           fill_percentage_,
                                           adversarial_distribution_,
                                           bv,
                                           rank_positions,
                                           select1_positions);
      std::cout << result << std::endl;
      std::this_thread::sleep_for(5s);
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
               " queries (accepts SI units, default is 10000).");

  cp.add_bool('a',
              "adversierial_distribution",
              rsb.adversarial_distribution_,
              "Create a bit vector where the majority of set bits is at the "
              "end (default is false).");

  cp.add_string('n', "name", rsb.filter_name_, "Runs the named algorithm.");

  if (!cp.process(argc, argv)) {
    return -1;
  }

  rsb.run_benchmark_configuration();

  return 0;
}

/******************************************************************************/
