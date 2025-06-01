cmake_minimum_required(VERSION 4.0)

project(orampir)

add_executable(pir src/main.cpp)
add_library(poly SHARED src/poly.cpp)

target_link_libraries(pir poly)
