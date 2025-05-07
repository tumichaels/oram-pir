#include "poly.hpp"
#include <random>

// Sample from {-1, 0, 1}
void sample_small_poly(const Params& params, Poly& out) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 2);

    out.resize(params.poly_len);
    for (size_t i = 0; i < params.poly_len; ++i) {
        int r = dist(gen) - 1;
        out[i] = (r < 0) ? params.q - 1 : r;
    }
}

// Sample a random polynomial (linear hash) from R_q uniformly
void sample_random_poly(const Params& params, Poly& out) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint64_t> dist(0, params.q - 1);

    out.resize(params.poly_len);
    for (size_t i = 0; i < params.poly_len; ++i)
        out[i] = dist(gen);
}

// Encode message into R_q
void encode_message(const Params& params, const Poly& m, Poly& out) {
    out.resize(params.poly_len);
    for (size_t i = 0; i < params.poly_len; ++i) {
        out[i] = (__uint128_t)m[i] * params.q / params.t % params.q;
    }
}

// Decode from R_q to message mod t
void decode_message(const Params& params, const Poly& m, Poly& out) {
    out.resize(params.poly_len);
    for (size_t i = 0; i < params.poly_len; ++i) {
        out[i] = ((__uint128_t)m[i] * params.t + params.q / 2) / params.q % params.t;
    }
}

// Polynomial addition
void poly_add(const Params& params, Poly& res, const Poly& a, const Poly& b) {
    res.resize(params.poly_len);
    for (size_t i = 0; i < params.poly_len; ++i) {
        res[i] = mod_add(params, a[i], b[i]);
    }
}

// Polynomial subtraction
void poly_sub(const Params& params, Poly& res, const Poly& a, const Poly& b) {
    res.resize(params.poly_len);
    for (size_t i = 0; i < params.poly_len; ++i) {
        res[i] = mod_sub(params, a[i], b[i]);
    }
}

// KeyGen: outputs pk = (a, b), sk = s
void keygen(const Params& params, Poly& a, Poly& b, Poly& s) {
    sample_small_poly(params, s);
    sample_small_poly(params, a); // public a ∈ R_q

    Poly e;
    sample_small_poly(params, e);

    Poly tmp;
    poly_mul(params, tmp, a, s);
    poly_sub(params, b, e, tmp); // b = -a*s + e  ≡ e - a*s mod q
}

// Encrypt message m ∈ R_t → ciphertext (c0, c1)
void encrypt(const Params& params, const Poly& m, const Poly& a, const Poly& b, Poly& c0, Poly& c1) {
    Poly r, e1, e2;
    sample_small_poly(params, r);
    sample_small_poly(params, e1);
    sample_small_poly(params, e2);

    Poly m_encoded;
    encode_message(params, m, m_encoded);

    Poly ar, br;
    poly_mul(params, ar, a, r);
    poly_mul(params, br, b, r);

    poly_add(params, c0, ar, e1);            // c0 = a*r + e1
    poly_add(params, c1, br, e2);            // c1 = b*r + e2
    poly_add(params, c1, c1, m_encoded);     // c1 += encode(m)
}

// Decrypt ciphertext (c0, c1) → message m
void decrypt(const Params& params, const Poly& s, const Poly& c0, const Poly& c1, Poly& m_out) {
    Poly sc0, decoded;
    poly_mul(params, sc0, c0, s);
    poly_sub(params, decoded, c1, sc0);     // decoded ≡ c1 - c0*s ≡ e + encode(m)

    decode_message(params, decoded, m_out);
}