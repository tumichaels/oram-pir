#include "params.hpp"
#include "oram_client.hpp"

std::ORAMClient(const Params &p) {
    params = p;

    l = 0;
    temp = 1;
    while (l < params.n) 
}