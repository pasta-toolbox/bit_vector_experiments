#include <sdsl/bit_vectors.hpp>

int32_t main(int32_t argc, char const * const argv[]) {
  sdsl::bit_vector bv(10000, 0);

  sdsl::bit_vector::select_0_type bvs0(&bv);
  sdsl::bit_vector::select_1_type bvs1(&bv);

  sdsl::bit_vector::rank_0_type bvr0(&bv);
  sdsl::bit_vector::rank_1_type bvr1(&bv);
  
  return 0;
}
