#pragma once

#include <functional>
#include <string>
#include <cassert>
#include <list>
#include <iostream>

namespace SearchTree {

    template <typename KeyT, typename Comp = std::less<KeyT>>
    class SearchTree {
        private:

            struct Node {
                KeyT key_;
                Node *parent_ = nullptr, *right_ = nullptr, *left_ = nullptr;
                size_t height_ = 1;

                Node() {}
                Node(KeyT key) : key_(key) {}
                ~Node() = default;
            };

            Comp comp_;
            std::list<Node*> nodes;

            int get_height(Node *node) {
                return node!=nullptr?node->height_:0;
            }

            int calculate_balance_factor(Node *node) {
                assert(node != nullptr);

                return get_height(node->left_) - get_height(node->right_);
            }

            void set_height(Node *node) {
                int height_left = get_height(node->left_);
                int height_right = get_height(node->right_);
                node->height_ = (height_left>height_right?height_left:height_right) + 1;
            }

            Node* rotate_right(Node* node) {
                assert(node != nullptr);
                
                Node* left_copy = node->left_;
                node->left_  = node->left_->right_;
                left_copy->right_ = node;
                left_copy->parent_= node->parent_;
                node->parent_ = left_copy;

                set_height(node);
                set_height(left_copy);

                return left_copy;
            }

            Node* rotate_left(Node *node) {
                assert(node != nullptr);
                assert(node->right_ != nullptr);

                Node* right_copy = node->right_;
                node->right_ = node->right_->left_;
                right_copy->left_ = node;
                right_copy->parent_= node->parent_;
                node->parent_ = right_copy;

                set_height(node);
                set_height(right_copy);

                return right_copy;
            }

            Node* balance(Node *node) {
                assert(node != nullptr);

                set_height(node);
                if (calculate_balance_factor(node) > 1) {
                    if (node->left_ != nullptr && calculate_balance_factor(node->left_) < 0) {
                        rotate_left(node->left_);
                    }
                    return rotate_right(node);
                }
                else if (calculate_balance_factor(node) < -1) {
                    if (node->right_ != nullptr && calculate_balance_factor(node->right_) > 0) {
                        rotate_right(node->right_);
                    }
                    return rotate_left(node);
                }
                return node;
            }

            Node* insert_to_node(Node* node, KeyT key) {

                if (node == nullptr) {
                    node = create_node(key);
                    return node;
                }

                if (search(node, key) != nullptr) {
                    return node;
                }

                if (key < node->key_) {
                    node->left_ = insert_to_node(node->left_, key);
                }

                else {
                    node->right_ = insert_to_node(node->right_, key);
                }

                return balance(node);
            }

        public:
            Node *root_ = nullptr;

            SearchTree() {}

            Node* create_node(KeyT key) {
                Node* new_node = new Node(key);
                nodes.push_back(new_node);

                return new_node;
            }

            void insert(KeyT key) {
                root_ = insert_to_node(root_, key);
            }

            Node* search(Node *node, KeyT key) const {
                if (node == nullptr || key == node->key_) {
                    return node;
                }
                if (key < node->key_) {
                    return search(node->left_, key);
                }
                else {
                    return search(node->right_, key);
                }
            }

            Node* min(Node *node) const {
                assert(node != nullptr);

                if (node->left_ != nullptr) {
                    return min(node->left_);
                }
                return node;
            }

            Node* max(Node *node) const {
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
    };
}