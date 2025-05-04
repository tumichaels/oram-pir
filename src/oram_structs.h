#pragma once
// ---- Includes and Ring-LWE support ----
#include "ringlwe.h"
#include <array>
#include <optional>
#include <iostream>
using namespace std;

// ---- OramEntry ----
struct OramEntry {
    uint64_t id;
    Poly value;

    OramEntry() : id(UINT64_MAX), value(Poly(N,0)) {}
    OramEntry(uint64_t id, const Poly& val) : id(id), value(val) {}

    static OramEntry dummy() {
        return OramEntry{UINT64_MAX, Poly(N, 0)};
    }

    bool is_dummy() const {
        return id == UINT64_MAX;
    }
};

inline Ciphertext encrypt_entry(const OramEntry& entry, const PublicKey& pk) {
    Poly m(N, 0);
    m[0] = entry.id;
    for (size_t i = 0; i < entry.value.size(); ++i)
        m[i + 1] = entry.value[i];
    return encrypt(m, pk);
}

inline OramEntry decrypt_entry(const Ciphertext& ct, const SecretKey& sk) {
    Poly m = decrypt(ct, sk);
    uint64_t id = m[0];
    Poly val(m.begin() + 1, m.end());
    return OramEntry{id, val};
}

// ---- Bucket<Z> ----
template <size_t Z>
struct Bucket {
    std::array<OramEntry, Z> entries;

    Bucket() {
        for (auto& e : entries) {
            e = OramEntry::dummy();
        }
    }

    bool insert(const OramEntry& entry) {
        // TODO: implement logic
        return false;
    }

    std::optional<OramEntry> find(uint64_t id) const {
        // TODO: implement logic
        return std::nullopt;
    }

    bool remove(uint64_t id) {
        // TODO: implement logic
        return false;
    }
};
