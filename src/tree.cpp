#include "read_and_process.hpp"
#include <iostream>

int main() {
  hwt::SearchTree<int> tree;

  std::cout << read_and_process<hwt::SearchTree<int>, int>(tree);

  return 0;
}