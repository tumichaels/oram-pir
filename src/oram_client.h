#pragma once
#include "ringlwe.h"
#include "oram_structs.h"
#include "oram_protocol.h"
#include "oram_storage.h"
#include <vector>
#include <optional>

class OramClient {
    size_t num_levels;
    std::vector<Hash> level_hashes;
    std::mt19937_64 rng;

public:
    OramClient(size_t levels) : num_levels(levels), level_hashes(levels), rng(std::random_device{}()) {}

    const Hash& get_hash_for_level(size_t level) const {
        return level_hashes.at(level);
    }

    size_t levels() const {
        return num_levels;
    }

    std::optional<OramEntry> access(uint64_t id, const PublicKey& pk, const SecretKey& sk, OramStorage<4>& storage) {
        bool found = false;
        std::optional<OramEntry> result;
    
        for (size_t level = 0; level < level_hashes.size(); ++level) {
            size_t idx;
            if (!found) {
                idx = level_hashes[level].h[0] * id % storage.level_size(level);
            } else {
                std::uniform_int_distribution<size_t> dist(0, storage.level_size(level) - 1);
                idx = dist(rng);
            }
    
            OramRequest req{level, idx};
            OramResponse resp = storage.handle_request(req, pk);
    
            for (const auto& ct : resp.entries) {
                Poly dec = decrypt(ct, sk);
                if (!found && dec[0] == id) {
                    result = OramEntry{id, dec};
                    found = true;
                }
            }
        }
    
        return result;
    }
};
