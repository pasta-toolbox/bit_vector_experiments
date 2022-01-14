#include <array>

#include <sdsl/bit_vectors.hpp>

#include <sux/bits/Rank9Sel.hpp>

#include <bitmap.h>
#include <shared.h>

int32_t main(int32_t argc, char const * const argv[]) {
  sdsl::bit_vector bv(10000, 0);

  sdsl::bit_vector::select_0_type bvs0(&bv);
  sdsl::bit_vector::select_1_type bvs1(&bv);

  sdsl::bit_vector::rank_0_type bvr0(&bv);
  sdsl::bit_vector::rank_1_type bvr1(&bv);

  std::array<uint64_t, 1000> array;

  sux::bits::Rank9Sel rs(array.data(), array.size() * 64);

  BitmapPoppy bitmap(array.data(), array.size() * 64);
  
  return 0;
}
