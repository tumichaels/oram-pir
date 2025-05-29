#pragma once
#include <cstdint>

struct Params {
    public:
        // rlwe parameters
        uint64_t q;          // Modulus for polynomial arithmetic
        uint64_t pt_modulus; // Message modulus
        uint64_t poly_len;   // Degree of polynomials (must be power of 2)

        // mod-math parameters
        uint64_t root;       // Primitive root of unity for NTT
        uint64_t inv_root;   // Primitive root of unity for NTT
        __uint128_t b_const; // Barrett constant 
        uint64_t b_shift;    // Barrett shift amount

        // oram parameters
        uint64_t n;
        uint64_t mu;
        uint64_t z;

        void display();
};
