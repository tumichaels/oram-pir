#include <vector>
#include <iostream>

#include "params.hpp"
#include "poly.hpp"
#include "crypto.hpp"

#include "oram_storage.hpp"

ORAMStorage::ORAMStorage(const Params &p) {
    this->params = p;

    this->a = Poly();
    this->b = Poly();
    this->sk = Poly();
    keygen(this->params, this->a, this->b, this->sk);

    this->l = 0;
    storage = std::vector<std::vector<std::pair<Poly, Poly>>>();
    uint64_t cap = this->params.z;
    while (cap <= this->params.z * this->params.n) {
        this->storage.push_back(std::vector<std::pair<Poly, Poly>>(cap));
        cap *= this->params.mu;
        this->l++;
    }
    this->l--; // annoying, but necessary

    this->cache_write_idx = 0;
}

std::vector<std::pair<Poly,Poly>> ORAMStorage::read(uint64_t lvl, uint64_t idx) {
    const auto &read_start = this->storage[lvl].begin() + params.z * idx;
    // note: creates a copy
    auto cp = std::vector<std::pair<Poly, Poly>>(read_start, read_start + params.z);
    return cp;
}

void ORAMStorage::write_cache(const std::pair<Poly, Poly> &val) {
    this->storage[0][this->cache_write_idx] = val;
    this->cache_write_idx = (this->cache_write_idx + 1) % this->params.z;
    }

uint64_t ORAMStorage::get_cap(uint64_t lvl) {
    return storage[lvl].size();
}

uint64_t ORAMStorage::get_num_levels() {
    return this->l+1;
}

void ORAMStorage::replace_lvl(uint64_t lvl, std::vector<std::pair<Poly,Poly>> &t) {
    this->storage[lvl] = t;
}

void ORAMStorage::display() {
    // TODO
    std::cout << "num levels: " << this->l+1 << std::endl;
    for (size_t i = 0; i <= this->l; i++) {
        std::cout << "lvl " << i << "- cap: " << this->storage[i].size() << std::endl;
    }
}