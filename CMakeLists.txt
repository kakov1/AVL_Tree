cmake_minimum_required(VERSION 3.11)
project("AVL_Tree")

find_package(GTest REQUIRED)
find_program(GRAPHVIZ_DOT_EXECUTABLE dot)

if(NOT GRAPHVIZ_DOT_EXECUTABLE)
    message(FATAL_ERROR "Graphviz (dot) not found")
endif()

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(./src)
add_subdirectory(./tests)
