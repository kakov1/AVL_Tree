#include "read_and_process.hpp"
#include <iostream>

int main() {
    SearchTree::SearchTree<int> tree;

    std::cout << read_and_process<SearchTree::SearchTree<int>, int>(tree);

    auto it = tree.begin();
    for (int i = 0; i < 20; i++) {
        std::cout << it.current_->key_<<std::endl;
        it++;
    }

    return 0;
}