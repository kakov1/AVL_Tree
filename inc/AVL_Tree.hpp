#pragma once

#include <functional>
#include <cassert>
#include <deque>
#include <iostream>

namespace SearchTree {
    template <typename KeyT, typename is_less = std::less<KeyT>,
              typename is_equal = std::equal_to<KeyT>,
              typename is_bigger = std::greater<KeyT>>
    class SearchTree {
        private:
            class Node;

            Node* root_ = nullptr;
            size_t size_ = 0;

            class Node {
                public:
                    KeyT key_;
                    Node *parent_ = nullptr, *right_ = nullptr,
                         *left_ = nullptr;
                    size_t height_ = 0;
                    size_t size_ = 0;

                    Node() = default;
                    Node(KeyT key) : key_(key){};
                    ~Node() = default;
            };

            size_t get_height(Node* node) const {
                return node != nullptr ? node->height_ : 0;
            }

            size_t get_size(Node* node) const {
                return node != nullptr ? node->size_ : 0;
            }

            int calculate_balance_factor(Node* node) const {
                assert(node != nullptr);

                return get_height(node->left_) - get_height(node->right_);
            }

            void set_height(Node* node) const {
                int height_left = get_height(node->left_);
                int height_right = get_height(node->right_);
                node->height_ =
                    (height_left > height_right ? height_left : height_right) +
                    1;
            }

            void set_size(Node* node) const {
                node->size_ =
                    get_size(node->left_) + get_size(node->right_) + 1;
            }

            Node* set_parents(Node* node, Node* child_copy) {
                child_copy->parent_ = node->parent_;

                if (node->parent_ != nullptr) {
                    if (node->parent_->right_ == node) {
                        node->parent_->right_ = child_copy;
                    }
                    else {
                        node->parent_->left_ = child_copy;
                    }
                }

                node->parent_ = child_copy;

                return child_copy;
            }

            Node* rotate_right(Node* node) {
                assert(node != nullptr);

                Node* left_copy = node->left_;
                node->left_ = node->left_->right_;
                left_copy->right_ = node;

                set_parents(node, left_copy);

                if (node->left_ != nullptr) {
                    node->left_->parent_ = node;
                }

                set_height(node);
                set_height(left_copy);
                set_size(node);
                set_size(left_copy);

                return left_copy;
            }

            Node* rotate_left(Node* node) {
                assert(node != nullptr);
                assert(node->right_ != nullptr);

                Node* right_copy = node->right_;
                node->right_ = node->right_->left_;
                right_copy->left_ = node;

                set_parents(node, right_copy);

                if (node->right_ != nullptr) {
                    node->right_->parent_ = node;
                }

                set_height(node);
                set_height(right_copy);
                set_size(node);
                set_size(right_copy);

                return right_copy;
            }

            Node* balance(Node* node) {
                assert(node != nullptr);

                set_height(node);
                set_size(node);
                if (calculate_balance_factor(node) > 1) {
                    if (node->left_ != nullptr &&
                        calculate_balance_factor(node->left_) < 0) {
                        rotate_left(node->left_);
                    }
                    return rotate_right(node);
                }
                else if (calculate_balance_factor(node) < -1) {
                    if (node->right_ != nullptr &&
                        calculate_balance_factor(node->right_) > 0) {
                        rotate_right(node->right_);
                    }
                    return rotate_left(node);
                }
                return node;
            }

            size_t get_rank(Node* node, KeyT key) const {
                size_t rank = 0;

                while (node != nullptr) {
                    if (is_less()(node->key_, key) ||
                        is_equal()(node->key_, key)) {
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

            Node* insert_to_node(Node* node, Node* parent, KeyT key) {
                if (node == nullptr) {
                    node = create_node(key, parent);
                    return node;
                }

                Node* node_copy = node;

                while (true) {
                    if (is_less()(key, node->key_)) {
                        if (node->left_ == nullptr) {
                            node->left_ = create_node(key, node);
                            break;
                        }
                        node = node->left_;
                    }

                    else {
                        if (node->right_ == nullptr) {
                            node->right_ = create_node(key, node);
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

            void breadth_first_search(
                Node* node, void (SearchTree::*interact_with_node)(Node*)) {
                if (node == nullptr)
                    return;

                Node* cur_node = node;
                std::deque<Node*> queue;
                queue.push_front(node);

                while (!queue.empty()) {
                    if (cur_node->right_ != nullptr) {
                        queue.push_front(cur_node->right_);
                    }

                    if (cur_node->left_ != nullptr) {
                        queue.push_front(cur_node->left_);
                    }

                    (this->*interact_with_node)(cur_node);
                    queue.pop_back();

                    cur_node = queue.back();
                }
            }

            void delete_node(Node* node) { delete node; }

            void insert_node(Node* node) { insert(node->key_); }

        public:
            SearchTree() = default;

            SearchTree(const SearchTree& tree) {
                breadth_first_search(tree.root_, &SearchTree::insert_node);
            }

            SearchTree(SearchTree&& tree) {
                std::swap(root_, tree.root_);
                std::swap(size_, tree.size_);
            }

            SearchTree& operator=(const SearchTree& tree) {
                if (this != &tree) {
                    *this = tree;
                }

                return *this;
            }

            SearchTree& operator=(SearchTree&& tree) {
                if (this != &tree) {
                    std::swap(*this, tree);
                }

                return *this;
            }

            ~SearchTree() {
                breadth_first_search(root_, &SearchTree::delete_node);
            }

            Node* get_root() const { return root_; }

            size_t get_size() const { return size_; }

            Node* create_node(KeyT key, Node* parent) const {
                Node* new_node = new Node(key);
                new_node->parent_ = parent;
                new_node->size_++;
                new_node->height_++;

                return new_node;
            }

            void insert(KeyT key) {
                if (search(root_, key) != nullptr)
                    return;

                size_++;
                root_ = insert_to_node(root_, nullptr, key);
            }

            Node* search(Node* node, KeyT key) const {
                while (node != nullptr && !is_equal()(key, node->key_)) {
                    if (is_less()(key, node->key_)) {
                        node = node->left_;
                    }
                    else {
                        node = node->right_;
                    }
                }

                return node;
            }

            Node* min(Node* node) const {
                assert(node != nullptr);

                while (node->left_ != nullptr) {
                    node = node->left_;
                }

                return node;
            }

            Node* max(Node* node) const {
                assert(node != nullptr);

                while (node->right_ != nullptr) {
                    node = node->right_;
                }

                return node;
            }

            Node* next(Node* node) const {
                assert(node != nullptr);

                if (node->right_ != nullptr) {
                    return min(node->right_);
                }

                Node* cur_parent = node->parent_;
                while (cur_parent != nullptr && cur_parent->left_ != node) {
                    node = cur_parent;
                    cur_parent = cur_parent->parent_;
                }

                return cur_parent;
            }

            Node* prev(Node* node) const {
                assert(node != nullptr);

                if (node->left_ != nullptr) {
                    return max(node->left_);
                }

                Node* cur_parent = node->parent_;
                while (cur_parent != nullptr && cur_parent->right_ != node) {
                    node = cur_parent;
                    cur_parent = cur_parent->parent_;
                }

                return cur_parent;
            }

            Node* lower_bound(KeyT key) const {
                if (search(root_, key) != nullptr) {
                    return search(root_, key);
                }

                Node* cur_node = root_;
                Node* max_node = min(root_);

                if (is_bigger()(cur_node->key_, max_node->key_) &&
                    is_less()(cur_node->key_, key)) {
                    max_node = cur_node;
                }

                while (true) {
                    if (is_less()(key, cur_node->key_)) {
                        if (cur_node->left_ == nullptr)
                            break;
                        cur_node = cur_node->left_;
                    }
                    else {
                        if (cur_node->right_ == nullptr)
                            break;
                        cur_node = cur_node->right_;
                    }

                    if (is_bigger()(cur_node->key_, max_node->key_) &&
                        is_less()(cur_node->key_, key)) {
                        max_node = cur_node;
                    }
                }

                if (is_less()(key, max_node->key_))
                    return nullptr;

                return max_node;
            }

            Node* upper_bound(KeyT key) const {
                if (search(root_, key) != nullptr) {
                    return search(root_, key);
                }

                Node* cur_node = root_;
                Node* min_node = max(root_);

                if (is_less()(cur_node->key_, min_node->key_) &&
                    is_bigger()(cur_node->key_, key)) {
                    min_node = cur_node;
                }

                while (cur_node != nullptr) {
                    if (is_less()(key, cur_node->key_)) {
                        if (cur_node->left_ == nullptr)
                            break;
                        cur_node = cur_node->left_;
                    }
                    else {
                        if (cur_node->right_ == nullptr)
                            break;
                        cur_node = cur_node->right_;
                    }

                    if (is_less()(cur_node->key_, min_node->key_) &&
                        is_bigger()(cur_node->key_, key)) {
                        min_node = cur_node;
                    }
                }

                if (!is_less()(key, min_node->key_))
                    return nullptr;

                return min_node;
            }

            size_t range_query(KeyT left_border, KeyT right_border) const {
                Node* left_border_ptr = upper_bound(left_border);
                Node* right_border_ptr = lower_bound(right_border);

                if (left_border_ptr == nullptr ||
                    right_border_ptr == nullptr) {
                    return 0;
                }

                if (is_bigger()(left_border_ptr->key_,
                                right_border_ptr->key_)) {
                    return 0;
                }

                return distance(left_border_ptr->key_, right_border_ptr->key_);
            }
    };
}