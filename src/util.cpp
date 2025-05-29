#include "util.hpp"

std::vector<Poly> encode_data_as_polys(const Params &params, std::istream &stream) {
    std::vector<Poly> out;
    uint64_t idx = 0;

    while (true) {
        Poly buffer(params.poly_len);
        buffer[0] = idx++;
        uint64_t chunk_size = (params.poly_len - 1) * sizeof(uint64_t);

        stream.read(reinterpret_cast<char*>(buffer.data() + 1), chunk_size);
        std::streamsize bytes_read = stream.gcount();
        if (!stream && bytes_read == 0) break;

        size_t words_read = (bytes_read + sizeof(uint64_t) - 1) / sizeof(uint64_t);
        for (size_t i = 1 + words_read; i < params.poly_len; ++i)
            buffer[i] = 0;

        out.push_back(std::move(buffer));
    }
    return out;
}

Poly make_filler(const Params &params) {
    Poly out(params.poly_len);
    out[0] = 0xFFFF'FFFF'FFFF'FFFF;
    return out;
}

bool is_filler(const Params &params, Poly x) {
    return x[0] == 0xFFFF'FFFF'FFFF'FFFF;
}

bool is_dummy(const Params &params, Poly x) {
    return x[0] >= params.n;
}
