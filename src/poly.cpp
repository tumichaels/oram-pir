#include <vector>
#include <cstdint>
#include <cassert>
#include <algorithm>

#include "params.hpp"

using Poly = std::vector<uint64_t>;

// --- Barrett Reduction ---
uint64_t barrett_reduce(const Params &params, uint64_t a)
{
    __uint128_t x = (__uint128_t)a * params.b_const;
    uint64_t q_est = x >> params.b_shift;
    uint64_t r = a - q_est * params.q;
    return (r >= params.q) ? r - params.q : r;
}

// --- Modular Arithmetic ---
uint64_t mod_add(const Params &params, uint64_t a, uint64_t b)
{
    uint64_t res = a + b;
    return (res >= params.q) ? res - params.q : res;
}

uint64_t mod_sub(const Params &params, uint64_t a, uint64_t b)
{
    return (a >= b) ? a - b : params.q + a - b;
}

uint64_t mod_mul(const Params &params, uint64_t a, uint64_t b)
{
    __uint128_t prod = (__uint128_t)a * b;
    return barrett_reduce(params, prod % params.q);
}

uint64_t modpow(const Params &params, uint64_t base, uint64_t exp)
{
    uint64_t res = 1;
    while (exp)
    {
        if (exp & 1)
            res = (__uint128_t)res * base % params.q;
        base = (__uint128_t)base * base % params.q;
        exp >>= 1;
    }
    return res;
}

// --- Bit Reversal ---
void bit_reverse(const Params &params, Poly &a)
{
    uint64_t n = params.poly_len;
    for (uint64_t i = 1, j = 0; i < n; ++i)
    {
        uint64_t bit = n >> 1;
        while (j & bit)
        {
            j ^= bit;
            bit >>= 1;
        }
        j ^= bit;
        if (i < j)
            std::swap(a[i], a[j]);
    }
}

// --- Forward NTT ---
void ntt(const Params &params, Poly &a)
{
    bit_reverse(params, a);
    for (uint64_t len = 2; len <= params.poly_len; len <<= 1)
    {
        uint64_t wlen = modpow(params, params.root, (params.q - 1) / len);
        for (uint64_t i = 0; i < params.poly_len; i += len)
        {
            uint64_t w = 1;
            for (uint64_t j = 0; j < len / 2; ++j)
            {
                uint64_t u = a[i + j];
                uint64_t v = mod_mul(params, a[i + j + len / 2], w);
                a[i + j] = mod_add(params, u, v);
                a[i + j + len / 2] = mod_sub(params, u, v);
                w = mod_mul(params, w, wlen);
            }
        }
    }
}

// --- Inverse NTT ---
void intt(const Params &params, Poly &a)
{
    bit_reverse(params, a);
    uint64_t n = params.poly_len;
    uint64_t inv_n = modpow(params, n, params.q - 2);
    for (uint64_t len = 2; len <= n; len <<= 1)
    {
        uint64_t wlen = modpow(params, params.root, params.q - 1 - (params.q - 1) / len);
        for (uint64_t i = 0; i < n; i += len)
        {
            uint64_t w = 1;
            for (uint64_t j = 0; j < len / 2; ++j)
            {
                uint64_t u = a[i + j];
                uint64_t v = mod_mul(params, a[i + j + len / 2], w);
                a[i + j] = mod_add(params, u, v);
                a[i + j + len / 2] = mod_sub(params, u, v);
                w = mod_mul(params, w, wlen);
            }
        }
    }
    for (uint64_t i = 0; i < n; ++i)
    {
        a[i] = mod_mul(params, a[i], inv_n);
    }
}

// --- Polynomial Multiplication (NTT-based) ---
void poly_mul(const Params &params, Poly &res, const Poly &a, const Poly &b)
{
    Poly A = a, B = b;
    assert(a.size() == params.poly_len && b.size() == params.poly_len);
    ntt(params, A);
    ntt(params, B);
    res.resize(params.poly_len);
    for (uint64_t i = 0; i < params.poly_len; ++i)
    {
        res[i] = mod_mul(params, A[i], B[i]);
    }
    intt(params, res);
}
