#include "read_and_process.hpp"
#include <iostream>
#include <set>

int main() {
  std::set<int> keys;

  std::cout << read_and_process<std::set<int>, int>(keys);

  return 0;
}