cmake_minimum_required(VERSION 4.0)

project(oram-pir VERSION 0.1 LANGUAGES CXX)

add_executable(oram-pir 
    src/crypto.cpp
    src/crypto.hpp
    src/main.cpp
    src/oram_client.cpp
    src/oram_client.hpp
    src/oram_storage.cpp
    src/oram_storage.hpp
    src/params.cpp
    src/params.hpp
    src/poly.cpp
    src/poly.hpp
    src/testing.hpp
    src/util.cpp
    src/util.hpp
)

find_package(OpenSSL REQUIRED)
target_link_libraries(oram-pir PRIVATE OpenSSL::Crypto)

target_compile_features(oram-pir PRIVATE cxx_std_17)
target_compile_options(oram-pir PRIVATE -Wall -Wextra -Wpedantic)