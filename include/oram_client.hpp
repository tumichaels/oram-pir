#pragma once
#include <vector>

#include "params.hpp"
#include "poly.hpp"
#include "oram_storage.hpp"

class ORAMClient {
    private:
        Params params;

        uint64_t t;                     // access counter
        uint64_t l;                     // lvls numbered 0..l inclusive

        std::vector<LinearHash> lvl_hashes;

    public:
        Poly sk;
        Poly a, b;
        ORAMClient(const Params &p);
        Poly read_index(ORAMStorage &s, uint64_t idx);

        std::vector<Poly> 
        build_table(
                ORAMStorage &s, 
                uint64_t lvl, 
                std::vector<Poly> vals // deduplicated ptext
        );

        void display();
};