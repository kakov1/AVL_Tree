add_executable(test_tree test_tree.cpp)

target_include_directories(test_tree PUBLIC ../inc/)
target_link_libraries(test_tree gtest::gtest)