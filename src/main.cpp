#include <iostream>
#include <vector>
#include <chrono>
#include <random>

// int main()
// {
//     uint64_t N = 1000;
//     uint64_t num_accesses = 1000;

//     // 1. generate random DB
//     unsigned seed = 83;
//     std::mt19937 rng( seed );  
//     std::uniform_int_distribution<uint64_t> data_dist(0, 1000);
//     std::vector<uint64_t> DB(N);
//     for (auto &e : DB) e = data_dist(rng);

//     // 2. init PIR
//     auto t1 = std::chrono::high_resolution_clock::now();
//         // perform init

//     auto t2 = std::chrono::high_resolution_clock::now();
//     std::cout << "time elapsed to initialize PIR: "
//               << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms\n";

    
   
//     // 3. Query PIR
//     // generating all random queries
//     std::vector<uint64_t> queries(num_accesses);
//     std::uniform_int_distribution<uint64_t> query_dist(0, N);
//     std::vector<uint64_t> queries(num_accesses);
//     for (auto &e : queries) e = query_dist(rng);

//     t1 = std::chrono::high_resolution_clock::now();
//     for (uint64_t i = 0; i < num_accesses; i++) {
//        //   a. make_query
//        //   b. answer
//        //   c. decode_query 
//     }
//     t2 = std::chrono::high_resolution_clock::now();
//     std::cout << "time elapsed for "
//               << num_accesses << " accesses: "
//               << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms\n";
// }

#include "testing.hpp"
int main() {
    // IS_TRUE(test_barrett_reduce);
    // IS_TRUE(test_mod_mul);
    // IS_TRUE(test_modpow);
    // IS_TRUE(test_ntt);
    // IS_TRUE(test_bit_reverse);
    // IS_TRUE(test_poly_basic);
    // IS_TRUE(test_encode_as_poly);
    // IS_TRUE(test_crypto_basic);
    // // IS_TRUE(test_ORAMClient_init);
    IS_TRUE(test_ORAMAccess);
    // IS_TRUE(test_ORAMStorage_init);

    return 0;
}