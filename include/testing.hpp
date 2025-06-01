#pragma once
#include <vector>
#include <sstream>
#include <iostream>

#include "params.hpp"
#include "poly.hpp"
#include "util.hpp"
#include "crypto.hpp"
#include "oram_client.hpp"
#include "oram_storage.hpp"

#define IS_TRUE(x)                                                         \
    do {                                                                   \
        if (!(x()))                                                        \
            std::cout << __FUNCTION__ << " failed on line "                \
                      << __LINE__ << std::endl;                            \
        else                                                               \
            std::cout << #x << " passed" << std::endl;                     \
    } while (0)

void poly_print(const Params &params, const Poly &p) {
    for (const auto &c : p) {
        std::cout << c << " ";
    }
}

Params get_test_params() {
    uint64_t q = 998244353;
    __uint128_t b_const = (__uint128_t(1) << 64) / q;
    return Params {
        .q = 998244353,         // prime ring
        .pt_modulus = 1024,
        .poly_len = 64,          // po2 b/c of NTT algo (should be at least 1024)
        .root = 3,
        .inv_root = 332748118,
        .b_shift = 64,          // width of vals
        .b_const = b_const,     // (1 << b_shift) / q

        .n = 16,
        .mu = 2,
        .z = 4,
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
    bit_reverse(params, a);
    // for (int i = 0; i < 8; i++) {
    //     std::cout << a[i] << " ";
    //     // if (a[i] != a_ans[i]) {
    //     //     return false;
    //     // }
    // }
    // std::cout << std::endl;
    Poly a_ans = {0, 4, 2, 6, 1, 5, 3, 7};
    for (int i = 0; i < 8; i++) {
        if (a[i] != a_ans[i]) {
            return false;
        }
    }
    return true;
}

bool test_add_poly() {
    Params params = get_test_params();
    Poly a = {0, 1, 2, 3, 0, 499122176, 0, 0};
    Poly b = {0, 1, 81, 81, 0, 499122177, 10, 1};
    Poly exp = {0, 2, 83, 84, 0, 0, 10, 1};
    
    poly_add(params, a, a, b);
    for (uint64_t i = 0; i < params.poly_len; i++) {
        if (a[i] != exp[i]) {
            return false;
        }
    }
    return true;
}

bool test_ntt() {
    Params params = get_test_params();
    Poly a = {0, 11, 2, 0, 0, 0, 0, 0};
    Poly exp = {0, 11, 2, 0, 0, 0, 0, 0};
    ntt(params, a);
    intt(params, a);

    for (uint64_t i = 0; i < params.poly_len; i++) {
        if (a[i] != exp[i]) {
            return false;
        }
    }
    return true;
}

bool test_mul_poly() {
    // fails to multiply powers with same parity, ie
    // x^2 * x^4 or x^3 * x^3
    Params params = get_test_params();
    Poly a = Poly(params.poly_len, 0);
    Poly b = Poly(params.poly_len, 0);
    a[1] = 1; b[1] = 1;
    Poly res;
    Poly exp = Poly(params.poly_len, 0);
    exp[2] = 1;

    poly_mul(params, res, a, b);
    // poly_print(params, res); std::cout << std::endl;
    for (uint64_t i = 0; i < params.poly_len; i++) {
        if (res[i] != exp[i]) {
            return false;
        }
    }
    return true;
}

bool test_poly_basic() {
    Params params = get_test_params();

    std::cout << "Basic Crypto Tests:" << std::endl;

    if (!test_add_poly()) return false;
    std::cout << "\tbasic add -- PASSED" << std::endl;

    if (!test_mul_poly()) return false;
    std::cout << "\tbasic mul -- PASSED" << std::endl;

    return true;
}

bool test_crypto_basic() {
    Params params = get_test_params();
    return true;
}

bool test_ORAMClient_init() {
    Params params = get_test_params();
    std::cout << "using parameters:\n";
    params.display();
    ORAMClient oc = ORAMClient(params);
    oc.display();
    return true;
}

bool test_ORAMStorage_init() {
    Params params = get_test_params();
    ORAMStorage os = ORAMStorage(params);
    os.display();;
    return true;
}

bool test_ORAMAccess() {
    Params params = get_test_params();
    params.display(); std::cout << std::endl;

    size_t num_items =  3; 

    std::stringstream ss;
    for (uint64_t i = 0; i < 63 * num_items; i++) {
        ss.write(reinterpret_cast<const char*>(&i), sizeof(uint64_t));
    }

    auto polys = encode_data_as_polys(params, ss);
    // poly_print(params, exp); std::cout<<std::endl;

    ORAMClient oc = ORAMClient(params);
    ORAMStorage os = ORAMStorage(params);

    for (size_t i = 0; i < os.get_num_levels()-1; i++) {
        auto table = oc.build_table(os, i, {});
        // encrypt table as vector of pairs
        std::vector<std::pair<Poly,Poly>> encrypted_table(table.size());
        for (size_t j = 0; j < table.size(); j++){
            encrypted_table[j].first = Poly(params.poly_len);
            encrypted_table[j].second = Poly(params.poly_len);
            encrypt(
                params,
                table[j], 
                oc.a,
                oc.b,
                encrypted_table[j].first, 
                encrypted_table[j].second
            );
        }
        os.replace_lvl(i, encrypted_table);
    }

    // build lowest level table
    std::cout << "building lowest level table..." << std::endl;
    auto table = oc.build_table(os,os.get_num_levels()-1,polys);
    std::vector<std::pair<Poly,Poly>> encrypted_table(table.size());
    for (size_t i=0; i < table.size(); i++){
        encrypted_table[i].first = Poly(params.poly_len);
        encrypted_table[i].second = Poly(params.poly_len);
        encrypt(
            params,
            table[i], 
            oc.a,
            oc.b,
            encrypted_table[i].first, 
            encrypted_table[i].second
        );
    }

    os.replace_lvl(os.get_num_levels()-1,encrypted_table);
    oc.display();
    os.display();

    std::cout << "==== double checking encryption ====" << std::endl;
    for (size_t i = 0; i < encrypted_table.size(); i++) {
        Poly m;
        auto &ct = encrypted_table[i];
        decrypt(params, oc.sk, ct.first, ct.second, m);
        std::cout << m[0] << std::endl;
        if (i % params.z == 3){
            std::cout << "-- " << std::endl;
        }
    }

    // sanity checks that have been dealth with
    // std::cout << "table.size(): " << table.size() << std::endl;
    // std::cout << "encrypted_table.size(): " << encrypted_table.size() << std::endl;
    // std::cout << "encrypted_table[0].first.size(): " << encrypted_table[0].first.size() << std::endl;

    for (size_t k = 0; k < params.z+1; k++) {
        std::cout << "search iteration: " << k << std::endl;
        for (size_t i = 0; i < num_items; i ++) {
            auto res = oc.read_index(os, i);
            for (uint64_t j = 0; j < params.poly_len; j++) {
                if (res[j] != polys[i][j]) {
                    return false;
                }
            }
        }
    }

    return true;
}

bool test_encode_as_poly() {
    Params params = get_test_params();

    std::stringstream ss;
    for (uint64_t i = 0; i < 630; i++) {
        ss.write(reinterpret_cast<const char*>(&i), sizeof(uint64_t));
    }

    auto polys = encode_data_as_polys(params, ss);

    for (size_t i = 0; i < polys.size(); ++i) {
        assert(polys[i][0] == i);
        for (size_t j = 1; j < params.poly_len; ++j) {
            size_t flat_idx = i * (params.poly_len - 1) + (j - 1);
            if (flat_idx < 10'000)
                assert(polys[i][j] == flat_idx);
        }
    }
    return true;
}