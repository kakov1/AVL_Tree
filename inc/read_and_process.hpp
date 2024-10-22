#pragma once

#include "AVL_Tree.hpp"
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <set>
#include <iterator>

template <typename KeysStorage, typename KeyT>
int range_query(const KeysStorage& keys, KeyT left_board, KeyT right_board) {
    using KeyIt = KeysStorage::iterator;
    KeyIt start = keys.lower_bound(left_board);
    KeyIt final = keys.upper_bound(right_board);
    return std::distance(start, final);
}

template <typename KeysStorage, typename KeyT>
class RangeQuery {
    public:
        static int get_range_query(const KeysStorage& keys, KeyT left_board,
                                   KeyT right_board) {
            return range_query(keys, left_board, right_board);
        }
};

template <typename KeyT>
class RangeQuery<std::set<KeyT>, KeyT> {
    public:
        static int get_range_query(const std::set<KeyT>& keys, KeyT left_board,
                                   KeyT right_board) {
            return range_query(keys, left_board, right_board);
        }
};

template <typename KeyT>
class RangeQuery<SearchTree::SearchTree<KeyT>, KeyT> {
    public:
        static int get_range_query(const SearchTree::SearchTree<KeyT>& keys,
                                   KeyT left_board, KeyT right_board) {
            return keys.range_query(left_board, right_board);
        }
};

template <typename KeysStorage, typename KeyT, typename Comp = std::less<KeyT>>
std::string read_and_process(KeysStorage& keys) {
    std::string result;
    Comp comp_;

    std::string request;
    KeyT key;
    KeyT left_border, right_border;

    while (std::cin >> request) {
        if (request == "k") {
            std::cin >> key;
            keys.insert(key);
        }

        else if (request == "q") {
            std::cin >> left_border >> right_border;
            if (comp_(right_border, left_border)) continue;
            result +=
                std::to_string(RangeQuery<KeysStorage, KeyT>::get_range_query(
                    keys, left_border, right_border)) +
                " ";
        }
    }

    result += "\n";

    return result;
}