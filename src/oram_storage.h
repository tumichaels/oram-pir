#pragma once
#include "oram_structs.h"
#include "oram_protocol.h"
#include <vector>
#include <cmath>

template <size_t Z>
class OramStorage {
    using BucketType = Bucket<Z>;
    std::vector<std::vector<BucketType>> levels;

public:
    OramStorage(size_t N_slots) {
        size_t level = 0;
        while ((1ULL << level) < N_slots) {
            ++level;
        }
        for (size_t i = 0; i <= level; ++i) {
            size_t num_buckets = 1ULL << i;
            levels.emplace_back(num_buckets); // level i has 2^i buckets
        }
    }

    size_t num_levels() const {
        return levels.size();
    }

    size_t level_size(size_t lvl) const {
        return levels[lvl].size();
    }

    BucketType& get_bucket(size_t lvl, size_t index) {
        return levels[lvl][index];
    }

    const BucketType& get_bucket(size_t lvl, size_t index) const {
        return levels[lvl][index];
    }

    OramResponse handle_request(const OramRequest& req, const PublicKey& pk) {
        OramResponse resp;
        const auto& bucket = levels[req.level][req.bucket_index];
        resp.entries.reserve(Z);
        for (const auto& entry : bucket.entries) {
            resp.entries.push_back(encrypt(entry.value, pk));
        }
        return resp;
    }
};
