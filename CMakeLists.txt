cmake_minimum_required(VERSION 3.11)
project("AVL_Tree")

find_package(GTest REQUIRED)

set(CMAKE_CXX_STANDARD 20)


add_library(avl_tree INTERFACE)

target_include_directories(avl_tree INTERFACE ./inc)

add_subdirectory(src)
add_subdirectory(tests)
