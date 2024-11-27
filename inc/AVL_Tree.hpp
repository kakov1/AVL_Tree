#pragma once

#include <functional>
#include <cassert>
#include <deque>
#include <iostream>
#include <memory>

namespace SearchTree {
	template <typename KeyT, typename Comp = std::less<KeyT>>
	class SearchTree final {
		private:

		struct Node;

		using NodePtr = std::shared_ptr<Node>;
		using ParentPtr = std::weak_ptr<Node>;

		NodePtr root_ = nullptr;
		size_t size_ = 0;

		struct Node final {
			KeyT key_;
			ParentPtr parent_;
			NodePtr right_ = nullptr, left_ = nullptr;
			size_t height_ = 0;
			size_t size_ = 0;

			Node() = default;
			Node(const KeyT& key) : key_(key){};
			~Node() noexcept = default;
		};

		class SearchTreeIt final {
			public:

			NodePtr current_ = nullptr;

			public:

            SearchTreeIt() = default;
			SearchTreeIt(const NodePtr& node) : current_{node} {}

			SearchTreeIt& operator++() {
                if (!current_) { // current_ && current_->parent_
                    throw std::out_of_range("Iterator out of range.");
                }

				if (current_->right_) {
					current_ = current_->right_;
					while (current_ && current_->left_) {
						current_ = current_->left_;
					}
					return *this;
				}

				NodePtr cur_parent = current_->parent_.lock();
				while (cur_parent && cur_parent->left_ != current_) {
					current_ = cur_parent;
					cur_parent = cur_parent->parent_.lock();
				}

				current_ = cur_parent;

				return *this;
			}

			SearchTreeIt operator++(int) {
				SearchTreeIt tmp = *this;
				++*this;
				return tmp;
			}
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

			node->parent_ = static_cast<ParentPtr>(child_copy);
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

		size_t get_rank(NodePtr node, KeyT key) const {
			size_t rank = 0;

			while (node) {
				if (Comp()(node->key_, key) ||
					(!Comp()(node->key_, key) && !Comp()(key, node->key_))) {
					rank += 1 + get_size(node->left_);
					node = node->right_;
				}
				else {
					node = node->left_;
				}
			}

			return rank;
		}

		size_t distance(KeyT left_border, KeyT right_border) const {
			return 1 + get_rank(root_, right_border) -
				   get_rank(root_, left_border);
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

		void
		breadth_first_search(NodePtr node,
							 void (SearchTree::*interact_with_node)(NodePtr)) {
			if (!node)
				return;

			NodePtr cur_node;
			std::deque<NodePtr> queue;
			queue.push_front(node);

			while (!queue.empty()) {
				cur_node = queue.back();

				if (cur_node->right_) {
					queue.push_front(cur_node->right_);
				}

				if (cur_node->left_) {
					queue.push_front(cur_node->left_);
				}

				(this->*interact_with_node)(cur_node);

				queue.pop_back();
			}
		}

		NodePtr create_node(KeyT key, NodePtr parent) const {
			NodePtr new_node = std::make_shared<Node>(key);
			new_node->parent_ = parent;
			new_node->size_++;
			new_node->height_++;

			return new_node;
		}

		void insert_node(NodePtr node) { insert(node->key_); }

		void swap(SearchTree& tree) noexcept {
			std::swap(root_, tree.root_);
			std::swap(size_, tree.size_);
		}

		public:

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

		NodePtr search(KeyT key) const { return search(root_, key); }

		NodePtr min(NodePtr node) const {
			while (node && node->left_) {
				node = node->left_;
			}

			return node;
		}

		NodePtr min() const { return min(root_); }

		NodePtr max(NodePtr node) const {
			while (node && node->right_) {
				node = node->right_;
			}

			return node;
		}

		NodePtr max() const { return max(root_); }

		NodePtr next(NodePtr node) const {
			assert(node);

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

		NodePtr prev(NodePtr node) const {
			assert(node);

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

		NodePtr lower_bound(KeyT key) const {
			if (search(root_, key)) {
				return search(root_, key);
			}

			NodePtr cur_node = root_;
			NodePtr max_node = min();

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
				return nullptr;

			return max_node;
		}

		NodePtr upper_bound(KeyT key) const {
			if (search(root_, key)) {
				return search(root_, key);
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
				return nullptr;

			return min_node;
		}

		size_t range_query(KeyT left_border, KeyT right_border) const {
			if (!root_)
				return 0;

			NodePtr left_border_ptr = upper_bound(left_border);
			NodePtr right_border_ptr = lower_bound(right_border);

			if (!left_border_ptr || !right_border_ptr) {
				return 0;
			}

			if (Comp()(right_border_ptr->key_, left_border_ptr->key_)) {
				return 0;
			}

			return distance(left_border_ptr->key_, right_border_ptr->key_);
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

		SearchTreeIt begin() const { return SearchTreeIt{min()}; }
		SearchTreeIt end() const { return SearchTreeIt{nullptr}; }
	};

}