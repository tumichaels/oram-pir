#pragma once
#include <vector>

#include "params.hpp"
#include "poly.hpp"

class ORAMClient {
    private:
        Params params;

        uint64_t l;                     // lvls numbered 0..l inclusive

        Poly a, b, sk;
        std::vector<Poly> lvl_hashes;

    public:
        ORAMClient(const Params &p);
};