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

# Compiler flags for used in experiments, i.e., -03, -march=native, and -DNDEBUG
set(CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -fdiagnostics-color=auto -O3 -march=native -DNDEBUG")

# tlx is used for its utility functionality. Add tlx subdirectory only,
# if it has not been added by another project already
if(NOT TARGET tlx)
  add_subdirectory(extlib/tlx)
endif()

# Add pasta::utils subdirectory only, if it has not been added by another
# project already
if (NOT TARGET pasta_bit_vector)
  add_subdirectory(extlib/bit_vector)
endif()

# Add pasta::utils subdirectory only, if it has not been added by another
# project already
if (NOT TARGET pasta_utils)
  add_subdirectory(extlib/utils)
endif()

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
  sdsl
  sux_rank_select
  poppy_rank_select
  tlx
  pasta_bit_vector
  pasta_utils
  pasta_memory_monitor)

################################################################################
