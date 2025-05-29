#pragma once
#include <vector>
#include <cstdint>
#include <cassert>
#include <algorithm>

#include "params.hpp"

using Poly = std::vector<uint64_t>;

// --- Barrett Reduction ---
uint64_t barrett_reduce(const Params &params, __uint128_t a);

// --- Modular Arithmetic ---
uint64_t mod_add(const Params &params, uint64_t a, uint64_t b);

uint64_t mod_sub(const Params &params, uint64_t a, uint64_t b);

uint64_t mod_mul(const Params &params, uint64_t a, uint64_t b);

uint64_t modpow(const Params &params, uint64_t base, uint64_t exp);


// --- Bit Reversal ---
void bit_reverse(const Params &params, Poly &a);

// --- Forward NTT ---
void ntt(const Params &params, Poly &a);

// --- Inverse NTT ---
void intt(const Params &params, Poly &a);

// --- Polynomial Multiplication (NTT-based) ---
void poly_mul(const Params &params, Poly &res, const Poly &a, const Poly &b);

// --- Polynomial Addition ---
void poly_add(const Params &params, Poly &res, const Poly &a, const Poly &b);

// --- Polynomial Subtraction ---
void poly_sub(const Params &params, Poly &res, const Poly &a, const Poly &b);