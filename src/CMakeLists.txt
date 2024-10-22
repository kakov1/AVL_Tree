add_executable(tree tree.cpp)
add_executable(set set.cpp)

target_link_libraries(tree PUBLIC cgraph gvc)

target_include_directories(tree PUBLIC ../inc/)
target_include_directories(set PUBLIC ../inc/)
