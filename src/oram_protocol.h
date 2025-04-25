#pragma once
#include "ringlwe.h"

struct OramRequest {
    size_t level;
    size_t bucket_index;
};

struct OramResponse {
    std::vector<Ciphertext> entries;
};
