#pragma once
#include <vector>

#include "params.hpp"
#include "poly.hpp"

class ORAMStorage {
    private:
        Params params;

        uint64_t l;                     // lvls numbered 0..l inclusive

        Poly sk;

        std::vector<std::vector<std::pair<Poly,Poly>>> storage;

    public:
        Poly a, b;
        ORAMStorage(const Params &p);
        // void setup_keys();
        uint64_t get_cap(uint64_t idx);
        std::vector<std::pair<Poly,Poly>> read(uint64_t lvl, uint64_t idx);
        void write(uint64_t lvl, uint64_t idx, const std::pair<Poly,Poly> &val);
        void replace_lvl(uint64_t lvl, std::vector<std::pair<Poly,Poly>> &t);
        void display();
        uint64_t get_num_levels();
};