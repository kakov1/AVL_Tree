add_executable(test_tree test_tree.cpp)
enable_testing()
include(GoogleTest)

target_include_directories(test_tree PUBLIC ../inc/)
target_link_libraries(test_tree gtest::gtest)

gtest_discover_tests(test_tree)
