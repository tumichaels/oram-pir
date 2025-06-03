#include <vector>
#include <iostream>
#include <numeric>
#include <iomanip>

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
    Poly out = make_dummy(this->params);

    uint64_t dummy_idx = this->params.n + this->t;

    uint64_t cap = 1; // is a hack, just expose the size
    for (uint64_t i = 0; i <= l; i++) {
        uint64_t to_hash = found ? dummy_idx : idx;
        uint64_t lvl_idx = this->lvl_hashes[i].hash(to_hash) % cap;
        cap *= this->params.mu;

        std::vector<std::pair<Poly,Poly>> resp = s.read(i, lvl_idx);

        std::cout << "hashed: " << to_hash << std::endl;
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

    // rerandomize
    std::pair<Poly,Poly> ct;
    encrypt(this->params, out, this->a, this->b, ct.first, ct.second);

    // write back
    s.write_cache(ct);

    // rebuild
    if (this->t % this->params.z == 0) {
        // find right level
        uint64_t i_bar = 0;
        uint64_t tau = this->params.z;
        while ((this->t % tau == 0) && (i_bar + 1 < this->l)) {
            i_bar++;
            tau *= this->params.mu;
        }
        uint64_t i_star = std::min(i_bar, this->l);

        // extract old values, sample new keys
        std::vector<Poly> ptext;
        for (size_t i = 0; i <= i_star; i++) {
            for (auto &ct : s.extract_lvl(i)) {
                Poly m;
                decrypt(this->params, this->sk, ct.first, ct.second, m);
                ptext.push_back(m);
            }
            this->lvl_hashes[i] = LinearHash();
        }        

        // delete duplicates, keep only unique
        std::stable_sort(ptext.begin(), ptext.end(), [&](Poly a, Poly b) {
            return a[0] < b[0];
        });

        ptext.erase(std::unique(ptext.begin(), ptext.end(), [](const Poly &a, const Poly &b) {
            return a[0] == b[0];
        }), ptext.end());

        for (size_t i = 0; i <= i_star; i++) {
            std::vector<Poly> table; 
            if (i != i_star)
                table = this->build_table(s, i, {});
            else   
                table = this->build_table(s, i, ptext);

            // encrypt table as vector of pairs
            std::vector<std::pair<Poly,Poly>> encrypted_table(table.size());
            for (size_t j = 0; j < table.size(); j++){
                encrypted_table[j].first = Poly(this->params.poly_len);
                encrypted_table[j].second = Poly(this->params.poly_len);
                encrypt(
                    this->params,
                    table[j], 
                    this->a,
                    this->b,
                    encrypted_table[j].first, 
                    encrypted_table[j].second
                );
            }
            s.replace_lvl(i, encrypted_table);
        }

        // still need to set the other tables to empty somehow --> did it same way as before
    }

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
    for (size_t i = 0; i < vals.size(); i++) {
        pos[i] = this->lvl_hashes[lvl].hash(vals[i][0]) % lvl_n;
        std::cout << "(" << vals[i][0] << "," << pos[i] << ") ";
    }
    
    std::cout << std::endl;

    // add filler
    pos.reserve(pos.size() + cap);
    vals.reserve(vals.size() + cap);
    for (size_t i = 0; i < lvl_n; i++) {
        for (size_t j = 0; j < params.z; j++) {
            pos.push_back(i);
            vals.push_back(make_filler(params));
        }
    }

    // std::cout << "all pos (including filler)" << std::endl;
    // for (auto &i : pos) std::cout << i << " ";
    // std::cout << std::endl;

    // sort by bin
    std::vector<size_t> indices(vals.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::sort(indices.begin(), indices.end(), [&](size_t a, size_t b) {
        if (pos[a] != pos[b]) return pos[a] < pos[b];
        return !is_filler(params,vals[a]) && is_filler(params,vals[b]);
    });

    // std::cout << "all indices (including filler)" << std::endl;
    // for (auto &i : indices) std::cout << i << " ";
    // std::cout << std::endl;

    std::vector<uint64_t> new_pos(vals.size());
    std::vector<Poly> new_vals(vals.size()); 

    // perform reordering
    for (size_t i = 0; i < vals.size(); i++) {
        new_pos[i] = pos[indices[i]];
        new_vals[i] = vals[indices[i]];
    }

    pos = std::move(new_pos);
    vals = std::move(new_vals);

    // std::cout << "new pos " << std::endl;
    // for (auto &i : pos) std::cout << i << " ";
    // std::cout << std::endl;

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

    // std::cout << "all reordered vals (including filler)" << std::endl;
    // for (auto &i : reordered_vals) std::cout << i[0] % this->params.n << "\n";
    // std::cout << std::endl;

    reordered_vals.resize(cap);
    return reordered_vals;
}

void ORAMClient::display() {
    std::cout << "number of levels: " << this->l + 1 << std::endl;
    std::cout << "hash keys:" << std::endl;
    for (size_t i = 0; i <= this->l; i++) {
        std::cout << "lvl " << i << ": " << std::hex;
        for (size_t j = 0; j < this->lvl_hashes[i].key.size(); j++) {
            std::cout << std::setw(2) << std::setfill('0') << static_cast<int>(this->lvl_hashes[i].key[j]);
        }
        std::cout << std::dec << std::endl;
    }
}