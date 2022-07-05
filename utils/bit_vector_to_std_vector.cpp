/*******************************************************************************
 * pasta/bit_vector_experiments/utils/bit_vector_to_std_vector.cpp
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

#include <fstream>
#include <iostream>
#include <pasta/bit_vector/bit_vector.hpp>
#include <string>
#include <tlx/cmdline_parser.hpp>
#include <vector>

int32_t main(int32_t argc, char const* const argv[]) {
  tlx::CmdlineParser cp;
  std::string input_path;
  std::string output_path = "last_bv.data";

  cp.set_description("Utility tool that reads a pasta::bit_vector and converts "
                     "it to an std::vector<size_t> by storing the positions of "
                     "all set bits.");
  cp.set_author("Florian Kurpicz <florian@kurpicz.org>");

  cp.add_param_string("input", input_path, "Path to input file.");
  cp.add_string('o', "output", output_path, "Path to the output file.");

  if (!cp.process(argc, argv)) {
    return -1;
  }

  // Read the bit vector
  std::ifstream file_in(input_path, std::ios::in | std::ios::binary);
  size_t size = 0;
  file_in.read(reinterpret_cast<char*>(&size), sizeof(size_t));
  pasta::BitVector bit_vector(size);
  unsigned long* input_bit_data = bit_vector.data().data();
  file_in.read(reinterpret_cast<char*>(input_bit_data),
               bit_vector.data().size_bytes());
  file_in.close();

  std::cout << "Loaded bit vector of size " << size << "\n";

  // Write the positions of the set bits to an std::vector<size_t>
  std::vector<size_t> vector_to_store;
  size_t count_ones = 0;

  for (size_t i = 0; i < bit_vector.size(); ++i) {
    if (bit_vector[i]) {
      vector_to_store.push_back(i);
      ++count_ones;
    }
  }

  std::cout << "Stored " << count_ones << " ones in the std::vector"
            << "\n";

  // Write the std::vector to the disk

  size_t const vector_size = vector_to_store.size();
  std::ofstream file_out(output_path, std::ios::out | std::ios::binary);
  file_out.write(reinterpret_cast<char const*>(&vector_size), sizeof(size_t));
  file_out.write(reinterpret_cast<char*>(vector_to_store.data()),
                 vector_size * sizeof(size_t));
  file_out.close();

  std::cout << "Written output to " << output_path << "\n";

  return 0;
}

/******************************************************************************/
