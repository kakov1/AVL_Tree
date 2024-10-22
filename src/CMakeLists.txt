add_executable(tree tree.cpp ../inc/AVL_Tree.hpp ../inc/tree_vizualization.hpp)
add_executable(set set.cpp)

target_link_libraries(tree PUBLIC cgraph gvc)

target_include_directories(tree PUBLIC ../inc/)
