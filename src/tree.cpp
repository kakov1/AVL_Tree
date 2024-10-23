#include "AVL_Tree.hpp"
#include "tree_vizualization.hpp"
#include "read_and_process.hpp"
#include <iostream>

int main() {
    SearchTree::SearchTree<int> tree;

    std::cout << read_and_process<SearchTree::SearchTree<int>, int>(tree);

    //vizualize_tree(tree);
  
    return 0;
}