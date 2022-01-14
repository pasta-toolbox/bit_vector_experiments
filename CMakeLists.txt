################################################################################
# CMakeLists.txt
#
# Copyright (C) 2021 Florian Kurpicz <florian@kurpicz.org>
#
# pasta::bit_vector_experiments is free software: you can redistribute
# it and/or modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation, either version
# 3 of the License, or (at your option) any later version.
#
# pasta::bit_vector_experiments is distributed in the hope that it
# will be useful, but WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
# the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with pasta::bit_vector_experiments. If not, see
# <http://www.gnu.org/licenses/>.
#
################################################################################

cmake_minimum_required(VERSION 3.4.1 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(pasta_bit_vector_experiments)

# Add the Succinct Data Structure Library (SDSL) to the benchmark.
if(NOT TARGET sdsl)
  add_subdirectory(extlib/sdsl-lite)
endif()

# Add all sux rank and select data structures to the benchmark.
add_library(sux_rank_select INTERFACE)
target_include_directories(sux_rank_select INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/extlib/sux)

# Add cs-poppy rank and select data structure to the benchmark.
add_library(poppy_rank_select
  ${CMAKE_CURRENT_SOURCE_DIR}/extlib/rankselect/bitmap.cc)
target_include_directories(sux_rank_select INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/extlib/rankselect)

add_executable(rank_select_benchmark
  ${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/rank_select_benchmark.cpp)
target_link_libraries(rank_select_benchmark
  sdsl)

################################################################################
