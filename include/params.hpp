#pragma once
#include <cstdint>

struct Params {
    uint64_t q;          // Modulus for polynomial arithmetic
    uint64_t t;          // Message modulus
    uint64_t poly_len;   // Degree of polynomials (must be power of 2)
    uint64_t root;       // Primitive root of unity for NTT
    uint64_t b_const;    // Barrett constant 
    uint64_t b_shift;    // Barrett shift amount


    uint64_t n;
    uint64_t mu;
    uint64_t z;
};

