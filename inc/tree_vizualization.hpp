#pragma once

#include "AVL_Tree.hpp"
#include <graphviz/gvc.h>
#include <fstream>
#include <string>

bool save_tree_png(std::string file_name){
    GVC_t *gvc;
    Agraph_t *g;
    FILE *fp;

    gvc = gvContext();
    fp = fopen((file_name + ".gv").c_str(), "r");
    g = agread(fp, 0);

    gvLayout(gvc, g, "dot");
    gvRender(gvc, g, "png", fopen((file_name + ".png").c_str(), "w"));

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


template<typename Node>
void vizualize_tree(const Node* node,
                    const std::string gv_file_name = "tree") {
    std::string gv_file_path = gv_file_name + ".gv";
    std::ofstream gv_file;
    gv_file.open(gv_file_path);

    auto left_child = node->left_;
    auto right_child = node->right_;

    gv_file << "digraph SearchTree {" << std::endl;

    vizualize_node(node, gv_file);

    gv_file << "}" << std::endl;

    save_tree_png(gv_file_name);
 
    gv_file.close();
}