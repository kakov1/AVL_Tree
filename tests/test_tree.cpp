#include "AVL_Tree.hpp"
#include "read_and_process.hpp"
#include <fstream>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <list>
#include <string>
#include <vector>

class TestTree : public testing::Test {
	protected:

	hwt::SearchTree<int> tree;

	TestTree() {
		for (int i = 0; i < 100; ++i) {
			if (i != 50)
				tree.insert(i);
		}
	}
};

const int OPEN_ERROR = -1;

std::string get_answer(int test_number) {
	std::string answer;

	std::ifstream answer_file("../../tests/tests/" +
							 std::to_string(test_number) + "answer.txt");

	answer_file.exceptions(std::ifstream::badbit);

	std::string buf;

	while (answer_file >> buf) {
		answer += (buf + " ");
	}

	return answer + "\n";
}

std::string test(int test_number) {
	std::ifstream test_file("../../tests/tests/" + std::to_string(test_number) +
							"test.txt");

	test_file.exceptions(std::ifstream::badbit);

	hwt::SearchTree<int> tree;

	std::string result, request;

	int key, left_border, right_border;

	while (test_file >> request) {
		if (request == "k") {
			test_file >> key;
			tree.insert(key);
		}

		else if (request == "q") {
			test_file >> left_border >> right_border;
			if (left_border >= right_border) {
				result += "0 ";
				continue;
			}

			auto start_it = tree.upper_bound(left_border);
			auto final_it = tree.lower_bound(right_border);

			if (start_it == tree.end() || final_it == tree.end()) {
				result += "0 ";
				continue;
			}

			result += std::to_string(std::distance(start_it, final_it)) + " ";
		}
	}

	result += "\n";

	return result;
}

TEST_F(TestTree, TestSearch) {
	ASSERT_TRUE(tree.search(100) == tree.end());
	ASSERT_TRUE(tree.search(99) != tree.end());
}

TEST_F(TestTree, TestSpecialNodes) {
	ASSERT_TRUE(*tree.min() == 0);
	ASSERT_TRUE(*tree.max() == 99);

	ASSERT_TRUE(*++tree.search(43) == *tree.search(44));
	ASSERT_TRUE(*--tree.search(43) == *tree.search(42));
}

TEST_F(TestTree, TestBounds) {
	ASSERT_TRUE(tree.lower_bound(101) == tree.search(99));
	ASSERT_TRUE(tree.upper_bound(101) == tree.end());

	ASSERT_TRUE(tree.upper_bound(-1) == tree.search(0));
	ASSERT_TRUE(tree.lower_bound(-1) == tree.end());

	ASSERT_TRUE(tree.upper_bound(50) == tree.search(51));
	ASSERT_TRUE(tree.lower_bound(50) == tree.search(49));
}

TEST_F(TestTree, TestRangeQueries) {
	ASSERT_TRUE(std::distance(tree.upper_bound(0), tree.lower_bound(99)) == 99);
	ASSERT_TRUE(std::distance(tree.upper_bound(-1), tree.lower_bound(100)) ==
				99);
	ASSERT_TRUE(std::distance(tree.upper_bound(-1), tree.lower_bound(10)) ==
				11);
	ASSERT_TRUE(std::distance(tree.upper_bound(89), tree.lower_bound(100)) ==
				11);
}

TEST_F(TestTree, TestConstructors) {
	hwt::SearchTree<int> tree_copy1 = tree;
	hwt::SearchTree<int> tree_copy2;
	hwt::SearchTree<int> tree_copy3;

	tree = tree;
	tree = std::move(tree);

	ASSERT_TRUE(tree == tree_copy1);

	for (int i = 13; i < 34; ++i) {
		tree_copy3.insert(i);
	}

	ASSERT_TRUE(tree == tree_copy1);
	ASSERT_FALSE(tree == tree_copy2);
	ASSERT_FALSE(tree == tree_copy3);

	hwt::SearchTree<int> tree_copy4 = std::move(tree_copy1);
	hwt::SearchTree<int> tree_copy5 = std::move(tree_copy3);

	ASSERT_TRUE(tree == tree_copy4);
	ASSERT_FALSE(tree == tree_copy5);
}

TEST_F(TestTree, TestAssign) {
	hwt::SearchTree<int> tree_copy1;
	hwt::SearchTree<int> tree2;
	hwt::SearchTree<int> tree_copy2;

	for (int i = 13; i < 34; ++i) {
		tree2.insert(i);
	}

	tree_copy1 = tree;
	tree_copy2 = tree2;

	ASSERT_TRUE(tree == tree_copy1);
	ASSERT_FALSE(tree == tree_copy2);

	hwt::SearchTree<int> tree_copy3 = std::move(tree_copy1);
	hwt::SearchTree<int> tree_copy4 = std::move(tree_copy2);

	ASSERT_TRUE(tree == tree_copy3);
	ASSERT_FALSE(tree == tree_copy4);
}

TEST(ete_tests, test1) { ASSERT_TRUE(test(1) == get_answer(1)); }

TEST(ete_tests, test2) { ASSERT_TRUE(test(2) == get_answer(2)); }

TEST(ete_tests, test3) { ASSERT_TRUE(test(3) == get_answer(3)); }

TEST(ete_tests, test4) { ASSERT_TRUE(test(4) == get_answer(4)); }

TEST(ete_tests, test5) { ASSERT_TRUE(test(5) == get_answer(5)); }

TEST(ete_tests, test6) { ASSERT_TRUE(test(6) == get_answer(6)); }

TEST(ete_tests, test7) { ASSERT_TRUE(test(7) == get_answer(7)); }

TEST(ete_tests, test8) { ASSERT_TRUE(test(8) == get_answer(8)); }

TEST(ete_tests, test9) { ASSERT_TRUE(test(9) == get_answer(9)); }

TEST(ete_tests, test10) { ASSERT_TRUE(test(10) == get_answer(10)); }

TEST(ete_tests, test11) { ASSERT_TRUE(test(11) == get_answer(11)); }

TEST(ete_tests, test12) { ASSERT_TRUE(test(12) == get_answer(12)); }

TEST(ete_tests, test13) { ASSERT_TRUE(test(13) == get_answer(13)); }

TEST(ete_tests, test14) { ASSERT_TRUE(test(14) == get_answer(14)); }

TEST(ete_tests, test15) { ASSERT_TRUE(test(15) == get_answer(15)); }

TEST(ete_tests, test16) { ASSERT_TRUE(test(16) == get_answer(16)); }

TEST(ete_tests, test17) { ASSERT_TRUE(test(17) == get_answer(17)); }

int main(int argc, char* argv[]) {
	::testing::InitGoogleMock(&argc, argv);
	return RUN_ALL_TESTS();
}