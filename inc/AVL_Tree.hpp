#pragma once

#include <functional>
#include <cassert>
#include <deque>
#include <iostream>
#include <memory>
#include <vector>
#include <cstddef>
#include <utility>

namespace SearchTree {
	template <typename KeyT, typename Comp = std::less<KeyT>>
	class SearchTree {
		private:

		struct Node;

		using NodePtr = std::unique_ptr<Node>;

		Node* root_ = nullptr;
		Comp comp_;
		std::vector<NodePtr> nodes_;

		struct Node final {
			private:

			std::size_t get_size(Node* node) const {
				return node ? node->size_ : 0;
			}

			public:

			KeyT key_;
			Node* right_ = nullptr;
			Node* left_ = nullptr;
			Node* parent_ = nullptr;
			std::size_t height_ = 0, size_ = 0;
			Comp comp_;

			template <typename... Args>
			Node(Args&&... args) : key_(std::forward<Args>(args)...){};

			std::size_t get_rank() const {
				std::size_t rank = get_size(left_);
				Node* node = parent_;

				while (node) {
					if (comp_(node->key_, key_)) {
						rank += 1 + get_size(node->left_);
					}

					node = node->parent_;
				}

				return rank;
			}
		};

		class SearchTreeIt final {
			public:

			using iterator_category = std::random_access_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = KeyT;
			using pointer = KeyT*;
			using reference = KeyT&;

			private:

			Node* current_;

			public:

			SearchTreeIt(Node* node = nullptr) : current_{node} {}

			SearchTreeIt& operator++() {
				current_ = SearchTree::next(current_);

				return *this;
			}

			SearchTreeIt operator++(int) {
				SearchTreeIt tmp = *this;
				++*this;
				return tmp;
			}

			SearchTreeIt& operator--() {
				current_ = SearchTree::prev(current_);

				return *this;
			}

			SearchTreeIt operator--(int) {
				SearchTreeIt tmp = *this;
				--*this;
				return tmp;
			}

			difference_type operator-(const SearchTreeIt& other) const {
				return 1 + current_->get_rank() - other.current_->get_rank();
			}

			bool operator==(const SearchTreeIt& rhs) const {
				if (!current_ && !rhs.current_)
					return true;

				if (!current_ && rhs.current_ || current_ && !rhs.current_)
					return false;

				return current_->key_ == rhs.current_->key_;
			}

			bool operator!=(const SearchTreeIt& rhs) const {
				return !(*this == rhs);
			}

			const KeyT& operator*() const { return current_->key_; }
		};

		std::size_t get_height(Node* node) const {
			return node ? node->height_ : 0;
		}

		std::size_t get_size(Node* node) const {
			return node ? node->size_ : 0;
		}

		int calculate_balance_factor(Node* node) const {
			assert(node);

			return get_height(node->left_) - get_height(node->right_);
		}

		void set_height(Node* node) const {
			int height_left = get_height(node->left_);
			int height_right = get_height(node->right_);
			node->height_ =
				(height_left > height_right ? height_left : height_right) + 1;
		}

		void set_size(Node* node) const {
			node->size_ = get_size(node->left_) + get_size(node->right_) + 1;
		}

		void set_parents(Node* node, Node* child_copy) {
			child_copy->parent_ = node->parent_;

			Node* parent = node->parent_;

			if (parent) {
				if (parent->right_ == node) {
					parent->right_ = child_copy;
				}
				else {
					parent->left_ = child_copy;
				}
			}

			node->parent_ = child_copy;
		}

		Node* rotate_right(Node* node) {
			assert(node);

			Node* left_copy = node->left_;
			node->left_ = node->left_->right_;
			left_copy->right_ = node;

			set_parents(node, left_copy);

			if (node->left_) {
				node->left_->parent_ = node;
			}

			set_height(node);
			set_height(left_copy);
			set_size(node);
			set_size(left_copy);

			return left_copy;
		}

		Node* rotate_left(Node* node) {
			assert(node);
			assert(node->right_);

			Node* right_copy = node->right_;
			node->right_ = node->right_->left_;
			right_copy->left_ = node;

			set_parents(node, right_copy);

			if (node->right_) {
				node->right_->parent_ = node;
			}

			set_height(node);
			set_height(right_copy);
			set_size(node);
			set_size(right_copy);

			return right_copy;
		}

		Node* balance(Node* node) {
			assert(node);

			set_height(node);
			set_size(node);
			if (calculate_balance_factor(node) > 1) {
				if (node->left_ && calculate_balance_factor(node->left_) < 0) {
					rotate_left(node->left_);
				}
				return rotate_right(node);
			}
			else if (calculate_balance_factor(node) < -1) {
				if (node->right_ &&
					calculate_balance_factor(node->right_) > 0) {
					rotate_right(node->right_);
				}
				return rotate_left(node);
			}
			return node;
		}

		Node* insert_node(Node* insertable) {
			if (!root_) {
				root_ = insertable;
				return root_;
			}

			Node* node = root_;
			Node* node_copy = node;

			while (node) {
				if (comp_(insertable->key_, node->key_)) {
					if (!node->left_) {
						node->left_ = insertable;
						insertable->parent_ = node;
						break;
					}
					node = node->left_;
				}

				else {
					if (!node->right_) {
						node->right_ = insertable;
						insertable->parent_ = node;
						break;
					}
					node = node->right_;
				}
			}

			while (node != node_copy) {
				balance(node);
				node = node->parent_;
			}

			return balance(node);
		}

		Node* tune_node(NodePtr& node) {
			Node* new_node_ptr = node.get();

			new_node_ptr->size_++;
			new_node_ptr->height_++;

			nodes_.push_back(std::move(node));

			return new_node_ptr;
		}

		Node* create_node(const KeyT& key) {
			NodePtr new_node = std::make_unique<Node>(key);

			return tune_node(new_node);
		}

		Node* create_node(KeyT&& key) {
			NodePtr new_node = std::make_unique<Node>(std::move(key));

			return tune_node(new_node);
		}

		Node* search(Node* node, const KeyT& key) const {
			while (node && (comp_(key, node->key_) || comp_(node->key_, key))) {
				if (comp_(key, node->key_)) {
					node = node->left_;
				}
				else {
					node = node->right_;
				}
			}

			return node;
		}

		static Node* next(Node* node) {
			if (!node)
				return node;

			if (node->right_) {
				return min(node->right_);
			}

			Node* cur_parent = node->parent_;
			while (cur_parent && cur_parent->left_ != node) {
				node = cur_parent;
				cur_parent = cur_parent->parent_;
			}

			return cur_parent;
		}

		static Node* prev(Node* node) {
			if (!node)
				return node;

			if (node->left_) {
				return max(node->left_);
			}

			Node* cur_parent = node->parent_;
			while (cur_parent && cur_parent->right_ != node) {
				node = cur_parent;
				cur_parent = cur_parent->parent_;
			}

			return cur_parent;
		}

		static Node* min(Node* node) {
			while (node && node->left_) {
				node = node->left_;
			}

			return node;
		}

		static Node* max(Node* node) {
			while (node && node->right_) {
				node = node->right_;
			}

			return node;
		}

		void swap(SearchTree& tree) noexcept {
			std::swap(root_, tree.root_);
			std::swap(nodes_, tree.nodes_);
		}

		public:

		SearchTree() = default;

		SearchTree(const SearchTree& tree) {
			for (auto&& key : tree) {
				insert(key);
			}
		}

		SearchTree(SearchTree&& tree) noexcept { swap(tree); }

		SearchTree& operator=(const SearchTree& tree) {
			if (this != &tree) {
				SearchTree<KeyT> tmp{tree};
				swap(tmp);
			}

			return *this;
		}

		SearchTree& operator=(SearchTree&& tree) noexcept {
			swap(tree);

			return *this;
		}

		bool operator==(const SearchTree<KeyT>& rhs) { return is_equal(rhs); }

		void insert(const KeyT& key) {
			if (search(root_, key)) {
				return;
			}

			root_ = insert_node(create_node(key));
		}

		void insert(KeyT&& key) {
			if (search(root_, key)) {
				return;
			}

			root_ = insert_node(create_node(std::move(key)));
		}

		template <typename... Args>
		void emplace(Args&&... args) {
			NodePtr node = std::make_unique<Node>(std::forward<Args>(args)...);

			if (search(root_, node->key_)) {
				return;
			}

			Node* node_ptr = node.get();
			tune_node(node);

			root_ = insert_node(node_ptr);
		}

		SearchTreeIt search(const KeyT& key) const {
			return SearchTreeIt{search(root_, key)};
		}

		SearchTreeIt min() const { return SearchTreeIt{min(root_)}; }

		SearchTreeIt max() const { return SearchTreeIt{max(root_)}; }

		SearchTreeIt lower_bound(const KeyT& key) const {
			if (!root_) {
				return end();
			}

			SearchTreeIt key_it = search(key);

			if (key_it != end()) {
				return key_it;
			}

			Node* cur_node = root_;
			Node* max_node = min(root_);

			if (comp_(max_node->key_, cur_node->key_) &&
				comp_(cur_node->key_, key)) {
				max_node = cur_node;
			}

			while (cur_node) {
				if (comp_(key, cur_node->key_)) {
					cur_node = cur_node->left_;
				}

				else {
					cur_node = cur_node->right_;
				}

				if (cur_node && comp_(max_node->key_, cur_node->key_) &&
					comp_(cur_node->key_, key)) {
					max_node = cur_node;
				}
			}

			if (comp_(key, max_node->key_))
				return end();

			return SearchTreeIt{max_node};
		}

		SearchTreeIt upper_bound(const KeyT& key) const {
			if (!root_) {
				return end();
			}

			SearchTreeIt key_it = search(key);

			if (key_it != end()) {
				return key_it;
			}

			Node* cur_node = root_;
			Node* min_node = max(root_);

			if (comp_(cur_node->key_, min_node->key_) &&
				comp_(key, cur_node->key_)) {
				min_node = cur_node;
			}

			while (cur_node) {
				if (comp_(key, cur_node->key_)) {
					cur_node = cur_node->left_;
				}

				else {
					cur_node = cur_node->right_;
				}

				if (cur_node && comp_(cur_node->key_, min_node->key_) &&
					comp_(key, cur_node->key_)) {
					min_node = cur_node;
				}
			}

			if (!comp_(key, min_node->key_))
				return end();

			return SearchTreeIt{min_node};
		}

		bool is_equal(const SearchTree& rhs) const {
			if (!root_ && !rhs.root_) {
				return true;
			}

			if ((!root_ && rhs.root_) || (root_ && !rhs.root_)) {
				return false;
			}

			Node* cur_node1 = root_;
			Node* cur_node2 = rhs.root_;

			std::deque<Node*> queue1;
			std::deque<Node*> queue2;

			queue1.push_front(cur_node1);
			queue2.push_front(cur_node2);

			while (!queue1.empty() && !queue2.empty()) {
				if (comp_(cur_node1->key_, cur_node2->key_) ||
					comp_(cur_node2->key_, cur_node1->key_)) {
					return false;
				}

				if (cur_node1->right_ && cur_node2->right_) {
					queue1.push_front(cur_node1->right_);
					queue2.push_front(cur_node2->right_);
				}

				else if (cur_node1->right_ || cur_node2->right_) {
					return false;
				}

				if (cur_node1->left_ && cur_node2->left_) {
					queue1.push_front(cur_node1->left_);
					queue2.push_front(cur_node2->left_);
				}

				else if (cur_node1->left_ || cur_node2->left_) {
					return false;
				}

				queue1.pop_back();
				queue2.pop_back();

				cur_node1 = queue1.back();
				cur_node2 = queue2.back();
			}

			return true;
		}

		SearchTreeIt begin() const { return SearchTreeIt{min(root_)}; }
		SearchTreeIt end() const { return SearchTreeIt{}; }
	};
}
