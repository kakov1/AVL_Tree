#include <iostream>
#include <set>
#include <iterator>

template<typename KeyT>
int range_query(const std::set<KeyT>& keys, KeyT left_board, KeyT right_board) {
    using KeyIt = std::set<KeyT>::iterator;
    KeyIt start = keys.lower_bound(left_board);
    KeyIt final = keys.upper_bound(right_board);
    return std::distance(start, final);
}

int main() {
    std::set<int> keys;

    std::string buf;
    int left_border, right_border;

    while (std::cin >> buf) {
        if (buf == "k") {
            std::cin >> buf;
            keys.insert(std::stoi(buf));
        }

        else if (buf == "q") {
            std::cin >> left_border >> right_border;
            if (left_border > right_border) {
                std::cout << "0 ";
            }
            else {
                std::cout << range_query(keys, left_border, right_border) << " ";
            }
        }
    }

    std::cout << std::endl;
  
    return 0;
}