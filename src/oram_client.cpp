#include <vector>
#include <iostream>
#include <numeric>

#include "params.hpp"
#include "poly.hpp"
#include "crypto.hpp"
#include "util.hpp"

#include "oram_storage.hpp"
#include "oram_client.hpp"

ORAMClient::ORAMClient(const Params &p) {
    this->params = p;

    this->a = Poly();
    this->b = Poly();
    this->sk = Poly();
    keygen(this->params, this->a, this->b, this->sk);

    this->t = 0;

    this->l = 0;
    uint64_t cap = params.z;
    while (cap <= this->params.z * this->params.n) {
        cap *= this->params.mu;
        this->lvl_hashes.push_back(LinearHash());
        this->l++;
    }
    this->l--; // annoying, but necessary
}

Poly ORAMClient::read_index(ORAMStorage &s, uint64_t idx) {
    this->t++;

    bool found = false;
    Poly out = Poly(); // should set this to dummy or something

    uint64_t dummy_idx = this->params.n + this->t;

    uint64_t cap = 1;
    for (uint64_t i = 0; i <= l; i++) {
        uint64_t to_hash = dummy_idx ? found : idx;
        uint64_t lvl_idx = this->lvl_hashes[i].hash(to_hash) % cap;
        cap *= params.mu;

        std::vector<std::pair<Poly,Poly>> resp = s.read(i, lvl_idx);

        std::cout << "searched: (" << i << "," << lvl_idx << ")" << std::endl;
        Poly m = Poly(this->params.poly_len);
        for (const auto &ct : resp) {
            decrypt(params, sk, ct.first, ct.second, m);
            std::cout << "found: " << m[0] << std::endl;

            if (m[0] == idx) {
                std::cout << "found idx " << idx << " at level " << i << std::endl;
                found = true;
                out = m;
            }
        }
    }

    // TODO: write back and rebuild

    return out;
}

std::vector<Poly> 
ORAMClient::build_table(
    ORAMStorage &s, 
    uint64_t lvl, 
    std::vector<Poly> vals // deduplicated ptext
){
    uint64_t cap = s.get_cap(lvl);
    uint64_t lvl_n = cap / params.z;

    // old vals with pos
    std::vector<uint64_t> pos(vals.size());
    for (size_t i; i < vals.size(); i++) {
        pos[i] = this->lvl_hashes[lvl].hash(vals[i][0]) % lvl_n;
    }

    // filler
    pos.reserve(pos.size() + cap);
    vals.reserve(vals.size() + cap);
    for (size_t i = 0; i < lvl_n; i++) {
        for (size_t j = 0; j < params.z; j++) {
            pos.push_back(i);
            vals.push_back(make_filler(params));
        }
    }

    // sort by bin
    std::vector<size_t> indices(vals.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::sort(indices.begin(), indices.end(), [&](size_t a, size_t b) {
        if (pos[a] != pos[b]) return pos[a] < pos[b];
        return !is_filler(params,vals[a]) && is_filler(params,vals[b]);
    });

    std::vector<uint64_t> new_pos(vals.size());
    std::vector<Poly> new_vals(vals.size()); 

    // perform reordering
    for (size_t i = 0; i < vals.size(); i++) {
        new_pos[i] = pos[indices[i]];
        new_vals[i] = vals[indices[i]];
    }

    pos = std::move(new_pos);
    vals = std::move(new_vals);

    // label excess
    std::vector<bool> is_excess(vals.size(), false);
    std::unordered_map<uint64_t, size_t> bin_counts;

    for (size_t i = 0; i < vals.size(); ++i) {
        uint64_t bin = pos[i];
        if (bin_counts[bin] < this->params.z)
            ++bin_counts[bin];
        else
            is_excess[i] = true;
    }

    // 2nd sort
    std::iota(indices.begin(), indices.end(), 0);
    std::stable_sort(indices.begin(), indices.end(), [&](size_t a, size_t b) {
        bool da = is_dummy(this->params, vals[a]);
        bool db = is_dummy(this->params, vals[b]);

        if (da != db) return !da;
        if (is_excess[a] != is_excess[b]) return !is_excess[a];
        if (!is_excess[a]) return pos[a] < pos[b];
        return !da && db;
    });
    
    // perform reordering according to index
    std::vector<Poly> reordered_vals(vals.size());
    for (size_t i = 0; i < vals.size(); i++) {
        reordered_vals[i] = vals[indices[i]];
    }

    reordered_vals.resize(cap);
    return reordered_vals;
}

void ORAMClient::display() {

}