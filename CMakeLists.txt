cmake_minimum_required(VERSION 3.15)
project(ChemLang)

set(CMAKE_CXX_STANDARD 14)

add_library(Tree ../Tree/Tree.cpp)

add_executable(ChemLang main.cpp)

target_link_libraries(ChemLang Tree)