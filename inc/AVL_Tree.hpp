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
            class Node {
                public:
                    KeyT key_;
                    Node *parent_ = nullptr, *right_ = nullptr, *left_ = nullptr;
                    size_t height_ = 1;

                    Node() = default;
                    Node(KeyT key) : key_(key) {};
                    ~Node() = default;
            };

            Comp comp_;
            std::list<Node*> nodes_;

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
                if (node->left_ != nullptr) {
                    node->left_->parent_ = node;
                }

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
                if (node->right_ != nullptr) {
                    node->right_->parent_ = node;
                }

                set_height(node);
                set_height(right_copy);

                return right_copy;
            }

            Node* balance(Node *node) {
                assert(node != nullptr);

                set_height(node);
                if (calculate_balance_factor(node) > 1) {
                    if (node->left_ != nullptr && calculate_balance_factor(node->left_) < 0) {
                        node->left_ = rotate_left(node->left_);
                    }
                    return rotate_right(node);
                }
                else if (calculate_balance_factor(node) < -1) {
                    if (node->right_ != nullptr && calculate_balance_factor(node->right_) > 0) {
                        node->right_ = rotate_right(node->right_);
                    }
                    return rotate_left(node);
                }
                return node;
            }

            Node* insert_to_node(Node* node, Node* parent, KeyT key) {

                if (node == nullptr) {
                    node = create_node(key);
                    node->parent_ = parent;
                    return node;
                }

                if (key < node->key_) {
                    node->left_ = insert_to_node(node->left_, node, key);
                }

                else {
                    node->right_ = insert_to_node(node->right_, node, key);
                }

                return balance(node);
            }

            Node* get_position(KeyT key) const {
                if (search(root_, key) != nullptr) {
                    return search(root_, key);
                }

                Node* cur_node = root_;

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
                }

                return cur_node;
            }

        public:
            Node *root_ = nullptr;

            SearchTree() = default;

            size_t get_size() const { return nodes_.size(); }

            Node* create_node(KeyT key) {
                Node* new_node = new Node(key);
                nodes_.push_back(new_node);

                return new_node;
            }

            void insert(KeyT key) { 
                if (search(root_, key) != nullptr) return;
                root_ = insert_to_node(root_, nullptr, key);
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
                    
                    if (cur_node->key_ > max_node->key_ && cur_node->key_ < key) {
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

                    if (cur_node->key_ < min_node->key_ && cur_node->key_ > key) {
                        min_node = cur_node;
                    }
                }

                return min_node;
            }

            size_t range_query(KeyT left_border, KeyT right_border) const {
                assert(max(root_) && min(root_));

                if (left_border > max(root_)->key_) return 0;
                if (right_border < min(root_)->key_) return 0;
                
                Node* left_border_node = upper_bound(left_border);
                Node* right_border_node = lower_bound(right_border);

                if (left_border_node == nullptr || right_border_node == nullptr) return 0;
                if (right_border_node->key_ < left_border_node->key_) return 0;
                if (right_border_node->key_ == left_border_node->key_) return 1;

                return distance(root_, left_border_node->key_, right_border_node->key_);
            }

            size_t distance(Node* node, KeyT left_border, KeyT right_border) const {     
                Node* next_left_border = next(search(node, left_border));
                Node* next_right_border = prev(search(node, right_border));

                if (next_left_border->key_ == right_border) {
                    return 2;
                }

                if (next_left_border->key_ == next_right_border->key_) {
                    return 3;
                }
                

                //if (search(node->right_, next_left_border->key_) &&
                    //search(node->right_, next_right_border->key_)) {
                    //node = node->right_;
                //}

                //if (search(node->left_, next_left_border->key_) &&
                    //search(node->left_, next_right_border->key_)) {
                    //node = node->left_;
                //}

                return 2 + distance(node, next_left_border->key_,
                                    next_right_border->key_);
            }
    };
}