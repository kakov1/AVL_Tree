#include "AVL_Tree.hpp"
#include <iostream>
#include <cmath>
#include <iomanip>

template<typename Node>
void print(Node *node) {
    if (node != nullptr) {
        print(node->left_);
        std::cout << node->key_ << std::endl;
        print(node->right_);
    }
}

template<typename Node>
void postorder(Node* p, int indent) {
    if(p != NULL) {
        if(p->right_) {
            postorder(p->right_, indent+4);
        }
        if (indent) {
            std::cout << std::setw(indent) << ' ';
        }
        if (p->right_) std::cout<<" /\n" << std::setw(indent) << ' ';
        std::cout<< p->key_ << "\n ";
        if(p->left_) {
            std::cout << std::setw(indent) << ' ' <<" \\\n";
            postorder(p->left_, indent+4);
        }
    }
}

int main() {
    SearchTree::SearchTree<int> tree;
    tree.insert(1);
    std::cout << "Tree structure:" << std::endl;
    postorder(tree.root_, 0);
    tree.insert(2);
    std::cout << "Tree structure:" << std::endl;
    postorder(tree.root_, 0);
    tree.insert(3);

    std::cout << "Tree structure:" << std::endl;
    postorder(tree.root_, 0);
        
    #ifndef A
    tree.insert(4);
    std::cout << "Tree structure:" << std::endl;
    postorder(tree.root_, 0);
    tree.insert(5);
    std::cout << "Tree structure:" << std::endl;
    postorder(tree.root_, 0);
    tree.insert(6);
    std::cout << "Tree structure:" << std::endl;
    postorder(tree.root_, 0);
    #endif
    
 
    return 0;
}