#include <vector>
#include "poly.hpp"

struct LinearHash {
    public:
        std::vector<uint8_t> key;

        LinearHash();
        uint64_t hash(uint64_t x);
};

// Sample from {-1, 0, 1}
void sample_small_poly(const Params& params, Poly& out);

// Sample a random polynomial (linear hash) from R_q uniformly
void sample_random_poly(const Params& params, Poly& out);

// Encode message into R_q
void encode_message(const Params& params, const Poly& m, Poly& out);

// Decode from R_q to message mod t
void decode_message(const Params& params, const Poly& m, Poly& out);

// KeyGen: outputs pk = (a, b), sk = s
void keygen(const Params& params, Poly& a, Poly& b, Poly& s);

// Encrypt message m ∈ R_t → ciphertext (c0, c1)
void encrypt(const Params& params, const Poly& m, const Poly& a, const Poly& b, Poly& c0, Poly& c1);

// Decrypt ciphertext (c0, c1) → message m
void decrypt(const Params& params, const Poly& s, const Poly& c0, const Poly& c1, Poly& m_out);