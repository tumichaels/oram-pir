#pragma once

#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <cassert>
#include <tuple>

using namespace std;

using Poly = vector<uint64_t>;

const uint64_t N = 1024;
const uint64_t Q = 1152921504606846977ULL; // 2^60 + 1
const uint64_t T = 257;
const uint64_t ROOT = 3;
uint64_t INV_N;
double sigma = 3.2;

struct PublicKey {
    Poly a, b;
};

struct SecretKey {
    Poly s;
};

struct Ciphertext {
    Poly u, v;
};

// --- Utility ---

uint64_t modinv(uint64_t a, uint64_t m) {
    int64_t m0 = m, x0 = 0, x1 = 1;
    while (a > 1) {
        uint64_t q = a / m;
        tie(a, m) = make_pair(m, a % m);
        tie(x0, x1) = make_pair(x1 - q * x0, x0);
    }
    return (x1 + m0) % m0;
}

uint64_t modpow(uint64_t base, uint64_t exp, uint64_t mod) {
    uint64_t res = 1;
    base %= mod;
    while (exp) {
        if (exp & 1) res = (__uint128_t)res * base % mod;
        base = (__uint128_t)base * base % mod;
        exp >>= 1;
    }
    return res;
}

void bit_reverse(Poly& a) {
    size_t n = a.size(), j = 0;
    for (size_t i = 1; i < n; i++) {
        size_t bit = n >> 1;
        while (j & bit) { j ^= bit; bit >>= 1; }
        j ^= bit;
        if (i < j) swap(a[i], a[j]);
    }
}

// --- NTT ---

void ntt(Poly& a) {
    bit_reverse(a);
    for (size_t len = 2; len <= N; len <<= 1) {
        uint64_t wlen = modpow(ROOT, (Q - 1) / len, Q);
        for (size_t i = 0; i < N; i += len) {
            uint64_t w = 1;
            for (size_t j = 0; j < len / 2; ++j) {
                uint64_t u = a[i + j];
                uint64_t v = (__uint128_t)a[i + j + len/2] * w % Q;
                a[i + j] = (u + v) % Q;
                a[i + j + len/2] = (u + Q - v) % Q;
                w = (__uint128_t)w * wlen % Q;
            }
        }
    }
}

void intt(Poly& a) {
    bit_reverse(a);
    for (size_t len = 2; len <= N; len <<= 1) {
        uint64_t wlen = modpow(modinv(ROOT, Q), (Q - 1) / len, Q);
        for (size_t i = 0; i < N; i += len) {
            uint64_t w = 1;
            for (size_t j = 0; j < len / 2; ++j) {
                uint64_t u = a[i + j];
                uint64_t v = a[i + j + len/2];
                a[i + j] = (u + v) % Q;
                a[i + j + len/2] = (__uint128_t)(u + Q - v) * w % Q;
                w = (__uint128_t)w * wlen % Q;
            }
        }
    }
    for (auto& x : a) x = (__uint128_t)x * INV_N % Q;
}

Poly poly_mul(const Poly& a, const Poly& b) {
    Poly A = a, B = b;
    ntt(A); ntt(B);
    for (size_t i = 0; i < N; ++i)
        A[i] = (__uint128_t)A[i] * B[i] % Q;
    intt(A);
    return A;
}

// --- Sampling ---

Poly sample_error() {
    static random_device rd;
    static mt19937_64 gen(rd());
    normal_distribution<double> dist(0.0, sigma);
    Poly e(N);
    for (size_t i = 0; i < N; ++i) {
        int64_t val = round(dist(gen));
        e[i] = (val % int64_t(Q) + Q) % Q;
    }
    return e;
}

// --- Ring-LWE PKE ---

pair<PublicKey, SecretKey> keygen() {
    SecretKey sk{sample_error()};
    PublicKey pk;
    pk.a.resize(N);
    static random_device rd;
    static mt19937_64 gen(rd());
    uniform_int_distribution<uint64_t> dist(0, Q - 1);
    for (auto& ai : pk.a) ai = dist(gen);
    Poly e = sample_error();
    Poly as = poly_mul(pk.a, sk.s);
    pk.b.resize(N);
    for (size_t i = 0; i < N; ++i)
        pk.b[i] = (as[i] + e[i]) % Q;
    return {pk, sk};
}

Ciphertext encrypt(const Poly& m, const PublicKey& pk) {
    Poly r = sample_error();
    Poly u = poly_mul(pk.a, r);
    Poly v = poly_mul(pk.b, r);
    for (size_t i = 0; i < N; ++i)
        v[i] = (v[i] + ((Q * m[i]) / T)) % Q;
    return {u, v};
}

Poly decrypt(const Ciphertext& ct, const SecretKey& sk) {
    Poly us = poly_mul(ct.u, sk.s);
    Poly m(N);
    for (size_t i = 0; i < N; ++i) {
        uint64_t val = (ct.v[i] + Q - us[i]) % Q;
        m[i] = ((val * T + Q/2) / Q) % T;
    }
    return m;
}

Ciphertext rerandomize(const Ciphertext& ct, const PublicKey& pk) {
    Poly r = sample_error();
    Poly delta_u = poly_mul(pk.a, r);
    Poly delta_v = poly_mul(pk.b, r);
    Ciphertext out;
    out.u.resize(N);
    out.v.resize(N);
    for (size_t i = 0; i < N; ++i) {
        out.u[i] = (ct.u[i] + delta_u[i]) % Q;
        out.v[i] = (ct.v[i] + delta_v[i]) % Q;
    }
    return out;
}

// --- Hashing ---

struct Hash {
    Poly h;

    Hash() {
        h.resize(N);
        static random_device rd;
        static mt19937_64 gen(rd());
        uniform_int_distribution<uint64_t> dist(0, Q - 1);
        for (auto& coeff : h)
            coeff = dist(gen);
    }

    Poly apply(const Poly& m) const {
        return poly_mul(h, m);
    }

    Ciphertext apply(const Ciphertext& ct) const {
        return {poly_mul(h, ct.u), poly_mul(h, ct.v)};
    }
};
