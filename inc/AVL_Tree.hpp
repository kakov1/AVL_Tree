#pragma once

#include <functional>
#include <cassert>
#include <deque>
#include <iostream>
#include <memory>

namespace SearchTree {
	template <typename KeyT, typename Comp = std::less<KeyT>>
	class SearchTree final {
		public:

		struct Node;

		using ParentPtr = std::weak_ptr<Node>;
		using NodePtr = std::shared_ptr<Node>;

		NodePtr root_ = nullptr;
		size_t size_ = 0;

		struct Node final {
			private:

			size_t get_size(NodePtr node) const {
				return node ? node->size_ : 0;
			}

			public:

			KeyT key_;
			ParentPtr parent_;
			NodePtr right_ = nullptr, left_ = nullptr;
			size_t height_ = 0, size_ = 0;

			Node() = default;
			Node(const KeyT& key) : key_(key){};
			~Node() noexcept = default;

			size_t get_rank() const {
				size_t rank = get_size(left_);
				NodePtr node = parent_.lock();

				while (node) {
					if (Comp()(node->key_, key_)) {
						rank += 1 + get_size(node->left_);
					}

					node = node->parent_.lock();
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

			NodePtr current_ = nullptr;

			public:

			SearchTreeIt() = default;
			explicit SearchTreeIt(const NodePtr& node) : current_{node} {}

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

			const NodePtr& operator*() const { return current_; }

			const NodePtr& operator->() const { return current_; }
		};

		size_t get_height(NodePtr node) const {
			return node ? node->height_ : 0;
		}

		size_t get_size(NodePtr node) const { return node ? node->size_ : 0; }

		int calculate_balance_factor(NodePtr node) const {
			assert(node);

			return get_height(node->left_) - get_height(node->right_);
		}

		void set_height(NodePtr node) const {
			int height_left = get_height(node->left_);
			int height_right = get_height(node->right_);
			node->height_ =
				(height_left > height_right ? height_left : height_right) + 1;
		}

		void set_size(NodePtr node) const {
			node->size_ = get_size(node->left_) + get_size(node->right_) + 1;
		}

		void set_parents(NodePtr node, NodePtr child_copy) {
			child_copy->parent_ = node->parent_;

			NodePtr parent = node->parent_.lock();

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

		NodePtr rotate_right(NodePtr node) {
			assert(node);

			NodePtr left_copy = node->left_;
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

		NodePtr rotate_left(NodePtr node) {
			assert(node);
			assert(node->right_);

			NodePtr right_copy = node->right_;
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

		NodePtr balance(NodePtr node) {
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

		NodePtr insert_to_node(NodePtr node, NodePtr parent, KeyT key) {
			if (!node) {
				node = create_node(key, parent);
				return node;
			}

			NodePtr node_copy = node;

			while (node) {
				if (Comp()(key, node->key_)) {
					if (!node->left_) {
						node->left_ = create_node(key, node);
						break;
					}
					node = node->left_;
				}

				else {
					if (!node->right_) {
						node->right_ = create_node(key, node);
						break;
					}
					node = node->right_;
				}
			}

			while (node != node_copy) {
				balance(node);
				node = node->parent_.lock();
			}

			return balance(node);
		}

		NodePtr create_node(KeyT key, NodePtr parent) const {
			NodePtr new_node = std::make_shared<Node>(key);
			new_node->parent_ = parent;
			new_node->size_++;
			new_node->height_++;

			return new_node;
		}

		NodePtr search(NodePtr node, KeyT key) const {
			while (node &&
				   (Comp()(key, node->key_) || Comp()(node->key_, key))) {
				if (Comp()(key, node->key_)) {
					node = node->left_;
				}
				else {
					node = node->right_;
				}
			}

			return node;
		}

		static NodePtr next(NodePtr node) {
			if (!node)
				return node;

			if (node->right_) {
				return min(node->right_);
			}

			NodePtr cur_parent = node->parent_.lock();
			while (cur_parent && cur_parent->left_ != node) {
				node = cur_parent;
				cur_parent = cur_parent->parent_.lock();
			}

			return cur_parent;
		}

		static NodePtr prev(NodePtr node) {
			if (!node)
				return node;

			if (node->left_) {
				return max(node->left_);
			}

			NodePtr cur_parent = node->parent_.lock();
			while (cur_parent && cur_parent->right_ != node) {
				node = cur_parent;
				cur_parent = cur_parent->parent_.lock();
			}

			return cur_parent;
		}

		static NodePtr min(NodePtr node) {
			while (node && node->left_) {
				node = node->left_;
			}

			return node;
		}

		static NodePtr max(NodePtr node) {
			while (node && node->right_) {
				node = node->right_;
			}

			return node;
		}

		void swap(SearchTree& tree) noexcept {
			std::swap(root_, tree.root_);
			std::swap(size_, tree.size_);
		}

		public:

		using iterator = SearchTreeIt;

		SearchTree() = default;

		SearchTree(const SearchTree& tree) {
			for (auto start_it = tree.begin(), end_it = tree.end();
				 start_it != end_it; ++start_it) {
				insert(start_it->key_);
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

		~SearchTree() noexcept = default;

		bool operator==(const SearchTree<KeyT>& rhs) { return is_equal(rhs); }

		void insert(KeyT key) {
			if (search(root_, key))
				return;

			size_++;
			root_ = insert_to_node(root_, nullptr, key);
		}

		SearchTreeIt search(KeyT key) const {
			return SearchTreeIt{search(root_, key)};
		}

		SearchTreeIt next(const SearchTreeIt& it) const {
			return SearchTreeIt{next(*it)};
		}

		SearchTreeIt prev(const SearchTreeIt& it) const {
			return SearchTreeIt{prev(*it)};
		}

		SearchTreeIt min(const SearchTreeIt& it) const {
			return SearchTreeIt{min(root_)};
		}

		SearchTreeIt max(const SearchTreeIt& it) const {
			return SearchTreeIt{max(root_)};
		}

		SearchTreeIt lower_bound(KeyT key) const {
			if (!root_) {
				return end();
			}

			if (search(key) != end()) {
				return search(key);
			}

			NodePtr cur_node = root_;
			NodePtr max_node = min(root_);

			if (Comp()(max_node->key_, cur_node->key_) &&
				Comp()(cur_node->key_, key)) {
				max_node = cur_node;
			}

			while (cur_node) {
				if (Comp()(key, cur_node->key_)) {
					cur_node = cur_node->left_;
				}

				else {
					cur_node = cur_node->right_;
				}

				if (cur_node && Comp()(max_node->key_, cur_node->key_) &&
					Comp()(cur_node->key_, key)) {
					max_node = cur_node;
				}
			}

			if (Comp()(key, max_node->key_))
				return end();

			return SearchTreeIt{max_node};
		}

		SearchTreeIt upper_bound(KeyT key) const {
			if (!root_) {
				return end();
			}

			if (search(key) != end()) {
				return search(key);
			}

			NodePtr cur_node = root_;
			NodePtr min_node = max(root_);

			if (Comp()(cur_node->key_, min_node->key_) &&
				Comp()(key, cur_node->key_)) {
				min_node = cur_node;
			}

			while (cur_node) {
				if (Comp()(key, cur_node->key_)) {
					cur_node = cur_node->left_;
				}

				else {
					cur_node = cur_node->right_;
				}

				if (cur_node && Comp()(cur_node->key_, min_node->key_) &&
					Comp()(key, cur_node->key_)) {
					min_node = cur_node;
				}
			}

			if (!Comp()(key, min_node->key_))
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

			NodePtr cur_node1 = root_;
			NodePtr cur_node2 = rhs.root_;

			std::deque<NodePtr> queue1;
			std::deque<NodePtr> queue2;

			queue1.push_front(cur_node1);
			queue2.push_front(cur_node2);

			while (!queue1.empty() && !queue2.empty()) {
				if (Comp()(cur_node1->key_, cur_node2->key_) ||
					Comp()(cur_node2->key_, cur_node1->key_)) {
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
		SearchTreeIt end() { return SearchTreeIt{nullptr}; }
	};
}