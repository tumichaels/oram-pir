#include <iostream>

#include "params.hpp"

void Params::display() {
    std::cout << "==== ring lwe parameters ====\n" ;
    std::cout << "q: " << q << "\n"
              << "t: " << pt_modulus << "\n"
              << "poly_len: " << poly_len << "\n";
    std::cout << "==== mod math parameters ====\n";
    std::cout << "root: " << root << "\n"
              << "inv_root: " << inv_root << "\n"
              << "b_const: " << (uint64_t)(b_const >> 64) << (uint64_t)b_const << "\n"
              << "b_shift: " << b_shift << "\n";
    std::cout << "====== ORAM parameters ======\n";
    std::cout << "n: " << n << "\n"
              << "mu: " << mu << "\n"
              << "z: " << z << "\n";
}