#pragma once

#include "AVL_Tree.hpp"
#include <graphviz/gvc.h>
#include <fstream>
#include <string>

bool save_tree_png(std::string file_path){
    GVC_t *gvc;
    Agraph_t *g;
    FILE *fp;

    gvc = gvContext();
    fp = fopen(file_path.c_str(), "r");
    g = agread(fp, 0);

    gvLayout(gvc, g, "dot");
    gvRender(gvc, g, "png", fopen("tree.png", "w"));

    gvFreeLayout(gvc, g);
    agclose(g);

    return gvFreeContext(gvc);
}

template<typename NodePtr>
void vizualize_node(const NodePtr node, std::ofstream &dot_file) {
    if (node == nullptr) return;

    if (node->left_ != nullptr) {
        dot_file << "   " << node->key_ << " -> " << node->left_->key_ << ";" << std::endl;
    }

    if (node->right_ != nullptr) {
        dot_file << "   " << node->key_ << " -> " << node->right_->key_ << ";" << std::endl;
    }
    
    vizualize_node(node->left_, dot_file);
    vizualize_node(node->right_, dot_file);
}


template<typename KeyT>
void vizualize_tree(const SearchTree::SearchTree<KeyT> &tree,
                    const std::string gv_file_path = "tree.gv") {
    std::ofstream gv_file;
    gv_file.open(gv_file_path);

    auto left_child = tree.root_->left_;
    auto right_child = tree.root_->right_;

    gv_file << "digraph SearchTree {" << std::endl;

    vizualize_node(tree.root_, gv_file);

    gv_file << "}" << std::endl;

    save_tree_png(gv_file_path);
 
    gv_file.close();
}