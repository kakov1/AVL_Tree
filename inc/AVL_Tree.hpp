#pragma once

#include <functional>
#include <string>
#include <cassert>
#include <list>
#include <iostream>
#include <deque>

namespace SearchTree {
    template <typename KeyT, typename Comp = std::less<KeyT>>
    class SearchTree {
        private:
            class Node {
                public:
                    KeyT key_;
                    Node *parent_ = nullptr, *right_ = nullptr,
                         *left_ = nullptr;
                    size_t height_ = 1;

                    Node() = default;
                    Node(KeyT key) : key_(key){};
                    ~Node() = default;
            };

            Node* root_ = nullptr;
            Comp comp_;

            int get_height(Node* node) {
                return node != nullptr ? node->height_ : 0;
            }

            int calculate_balance_factor(Node* node) {
                assert(node != nullptr);

                return get_height(node->left_) - get_height(node->right_);
            }

            void set_height(Node* node) {
                int height_left = get_height(node->left_);
                int height_right = get_height(node->right_);
                node->height_ =
                    (height_left > height_right ? height_left : height_right) +
                    1;
            }

            Node* rotate_right(Node* node) {
                assert(node != nullptr);

                Node* left_copy = node->left_;
                node->left_ = node->left_->right_;
                left_copy->right_ = node;
                left_copy->parent_ = node->parent_;

                if (node->parent_ != nullptr) {
                    if (node->parent_->right_ == node) {
                        node->parent_->right_ = left_copy;
                    }
                    else {
                        node->parent_->left_ = left_copy;
                    }
                }

                node->parent_ = left_copy;

                if (node->left_ != nullptr) {
                    node->left_->parent_ = node;
                }

                set_height(node);
                set_height(left_copy);

                return left_copy;
            }

            Node* rotate_left(Node* node) {
                assert(node != nullptr);
                assert(node->right_ != nullptr);

                Node* right_copy = node->right_;
                node->right_ = node->right_->left_;
                right_copy->left_ = node;
                right_copy->parent_ = node->parent_;

                if (node->parent_ != nullptr) {
                    if (node->parent_->right_ == node) {
                        node->parent_->right_ = right_copy;
                    }
                    else {
                        node->parent_->left_ = right_copy;
                    }
                }

                node->parent_ = right_copy;

                if (node->right_ != nullptr) {
                    node->right_->parent_ = node;
                }

                set_height(node);
                set_height(right_copy);

                return right_copy;
            }

            Node* balance(Node* node) {
                assert(node != nullptr);

                set_height(node);
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

            Node* insert_to_node(Node* node, Node* parent, KeyT key) {
                if (node == nullptr) {
                    node = create_node(key, parent);
                    return node;
                }

                Node* node_copy = node;

                while (true) {
                    if (comp_(key, node->key_)) {
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

            size_t distance(Node* node, KeyT left_border,
                             KeyT right_border) const {
                std::deque<Node*> queue;
                queue.push_front(node);

                int result = 0;

                while (!queue.empty()) {
                    Node* cur_node = queue.back();
                    queue.pop_back();

                    if (cur_node->key_ <= right_border && cur_node->key_ >= left_border) {
                        result++;
                        if (cur_node->left_ != nullptr) {
                            queue.push_front(cur_node->left_);
                        }

                        if (cur_node->right_ != nullptr) {
                            queue.push_front(cur_node->right_);
                        }
                    }

                    else if (cur_node->key_ < left_border && cur_node->right_) {
                        queue.push_front(cur_node->right_);
                    }

                    else if (cur_node->key_ > right_border && cur_node->left_) {
                        queue.push_front(cur_node->left_);
                    }
                }

                return result;
            }
            
            void breadth_first_search(Node* node, void (SearchTree::*interact_with_node)(Node*)) {
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

                    this->*interact_with_node(cur_node);
                    queue.pop_back();
                    
                    cur_node = queue.back();
                }
            }

            void delete_node(Node* node) {
                delete node;
            }

            void insert_node(Node* node) {
                insert(node->key_);
            }

        public:
            SearchTree() = default;

            SearchTree(const SearchTree& tree) {
                breadth_first_search(tree.root_, &SearchTree::insert_node);
            }

            ~SearchTree() {
                breadth_first_search(root_, &SearchTree::delete_node);
            }

            Node* get_root() const { return root_; }

            Node* create_node(KeyT key, Node* parent) {
                Node* new_node = new Node(key);
                new_node->parent_ = parent;

                return new_node;
            }

            void insert(KeyT key) {
                if (search(root_, key) != nullptr)
                    return;
                root_ = insert_to_node(root_, nullptr, key);
            }

            Node* search(Node* node, KeyT key) const {
                while (node != nullptr && key != node->key_) {
                    if (key < node->key_) {
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

                if (node->left_ != nullptr) {
                    return min(node->left_);
                }
                return node;
            }

            Node* max(Node* node) const {
                assert(node != nullptr);

                if (node->right_ != nullptr) {
                    return max(node->right_);
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

                if (cur_node->key_ > max_node->key_ && cur_node->key_ < key) {
                    max_node = cur_node;
                }

                while (true) {
                    if (key < cur_node->key_) {
                        if (cur_node->left_ != nullptr) {
                            cur_node = cur_node->left_;
                        }
                        else {
                            return max_node;
                        }
                    }
                    else {
                        if (cur_node->right_ != nullptr) {
                            cur_node = cur_node->right_;
                        }
                        else {
                            return max_node;
                        }
                    }

                    if (cur_node->key_ > max_node->key_ &&
                        cur_node->key_ < key) {
                        max_node = cur_node;
                    }
                }
            }

            Node* upper_bound(KeyT key) const {
                if (search(root_, key) != nullptr) {
                    return search(root_, key);
                }

                Node* cur_node = root_;
                Node* min_node = max(root_);

                if (cur_node->key_ < min_node->key_ && cur_node->key_ > key) {
                    min_node = cur_node;
                }

                while (true) {
                    if (key < cur_node->key_) {
                        if (cur_node->left_ != nullptr) {
                            cur_node = cur_node->left_;
                        }
                        else {
                            break;
                        }
                    }
                    else {
                        if (cur_node->right_ != nullptr) {
                            cur_node = cur_node->right_;
                        }
                        else {
                            break;
                        }
                    }

                    if (cur_node->key_ < min_node->key_ &&
                        cur_node->key_ > key) {
                        min_node = cur_node;
                    }
                }

                return min_node;
            }

            size_t range_query(KeyT left_border, KeyT right_border) const {

                return distance(root_, left_border, right_border);
            }
    };
}