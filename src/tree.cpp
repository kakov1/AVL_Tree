#include "AVL_Tree.hpp"
#include "tree_vizualization.hpp"
#include "read_and_process.hpp"
#include <iostream>
#include <chrono>

int main() {
    SearchTree::SearchTree<int> tree;

    std::cout << read_and_process<SearchTree::SearchTree<int>, int>(tree);

    SearchTree::SearchTree<int> tree_{tree}; 

    vizualize_tree(tree);
    //vizualize_tree(tree_, "tree_copy");
  
    return 0;
}