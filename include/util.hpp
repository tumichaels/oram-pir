#pragma once
#include <vector>
#include <iostream>
#include "poly.hpp"

std::vector<Poly> encode_data_as_polys(const Params &params, std::istream &stream);
Poly make_filler(const Params &params);
bool is_dummy(const Params &params, Poly x);
bool is_filler(const Params &params, Poly x);
