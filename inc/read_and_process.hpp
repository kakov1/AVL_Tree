#pragma once

#include "AVL_Tree.hpp"
#include <sstream>
#include <string>
#include <set>
#include <iterator>

template <typename KeysStorage, typename KeyT, typename Comp = std::less<KeyT>>
std::string read_and_process(KeysStorage& keys) {
	std::string result;
	Comp is_less;

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
			if (!is_less(left_border, right_border)) {
				result += "0 ";
				continue;
			}

			auto start_it = keys.upper_bound(left_border);
			auto final_it = keys.lower_bound(right_border);

			if (start_it == keys.end() ||
				final_it == keys.end()) {
				result += "0 ";
				continue;
			}

			result += std::to_string(std::distance(start_it, final_it)) + " ";
		}
	}

	result += "\n";

	return result;
}
