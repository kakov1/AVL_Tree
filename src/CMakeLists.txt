cmake_minimum_required(VERSION 3.11)
project(src)

set(CMAKE_CXX_STANDARD 20)

add_executable(main main.cpp ../inc/AVL_Tree.hpp)

target_include_directories(main PUBLIC ../inc/)

