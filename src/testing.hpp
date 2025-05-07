#pragma once
#include <iostream>

#include "params.hpp"
#include "poly.hpp"

#define IS_TRUE(x)                                                         \
    do {                                                                   \
        if (!(x()))                                                        \
            std::cout << __FUNCTION__ << " failed on line "                \
                      << __LINE__ << std::endl;                            \
        else                                                               \
            std::cout << #x << " passed" << std::endl;                     \
    } while (0)


Params get_test_params() {
    return Params {
        q: 83,              // prime ring
        poly_len: 8,        // po2 b/c of NTT algo
        root: 3, 
        b_const: 3,         // 2^8 / q
        b_shift: 8,
    };
}

bool test_barrett_reduce() {
    Params params = get_test_params();
    uint64_t a = 343;
    std::cout << "barret_reduce 343 mod 83: " << barrett_reduce(params, a) << std::endl;
    return barrett_reduce(params, a) == 11;
}

bool test_mod_mul() {
    Params params = get_test_params();
    uint64_t a = 343, b = 17;
    return mod_mul(params, a, b) == 21;
}

bool test_modpow() {
    Params params = get_test_params();
    uint64_t base = 5, exp = 17;
    return modpow(params, base, exp) == 76;
}

bool test_bit_reverse() {
    Params params = get_test_params();
    Poly a = {0, 1, 2, 3, 4, 5, 6, 7};
    std::cout << std::endl;
    Poly a_ans = {0, 4, 2, 6, 1, 5, 3, 7};
    for (int i = 0; i < 8; i++) {
        if (a[i] != a_ans[i]) {
            return false;
        }
    }
    return true;
}