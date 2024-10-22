#include "AVL_Tree.hpp"
#include "tree_vizualization.hpp"
#include <iostream>
#include <cmath>
#include <iomanip>
#include <string>
#include <sstream>

template<typename Node>
void print(Node *node) {
    if (node != nullptr) {
        print(node->left_);
        std::cout << node->key_ << std::endl;
        print(node->right_);
    }
}

size_t keys_between_borders(const SearchTree::SearchTree<int>& tree, int left_border, int right_border) {
    size_t count = 0;

    for (int i = left_border; i <= right_border; i++) {
        if (tree.search(tree.root_, i)) count++;
    }

    return count;
}

int main() {
    SearchTree::SearchTree<int> tree;

    std::string buf;
    int left_border, right_border;

    while (std::cin >> buf) {
        if (buf == "k") {
            std::cin >> buf;
            tree.insert(std::stoi(buf));
        }

        else if (buf == "q") {
            std::cin >> left_border >> right_border;
            if (left_border > right_border) {
                std::cout << "0 ";
            }
            else {
                std::cout << tree.range_query(left_border, right_border) << " ";
            }
        }
    }

    std::cout << std::endl;

    //vizualize_tree(tree.root_);
  
    return 0;
}