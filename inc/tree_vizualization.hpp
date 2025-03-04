#pragma once

#include "AVL_Tree.hpp"
#include <fstream>
#include <graphviz/gvc.h>
#include <iostream>
#include <string>

bool save_tree_png(std::string file_name) {
  GVC_t *gvc;
  Agraph_t *g;
  FILE *fp, *fw;

  gvc = gvContext();
  fp = fopen((file_name + ".gv").c_str(), "r");
  fw = fopen((file_name + ".png").c_str(), "w");
  g = agread(fp, 0);

  gvLayout(gvc, g, "dot");
  gvRender(gvc, g, "png", fw);

  gvFreeLayout(gvc, g);
  agclose(g);

  fclose(fw);
  fclose(fp);

  return gvFreeContext(gvc);
}

template <typename NodePtr>
void vizualize_node(const NodePtr node, std::ofstream &dot_file) {
  if (node == nullptr)
    return;

  if (node->left_ != nullptr) {
    dot_file << "   " << node->key_ << " -> " << node->left_->key_ << ";"
             << std::endl;
  }

  if (node->right_ != nullptr) {
    dot_file << "   " << node->key_ << " -> " << node->right_->key_ << ";"
             << std::endl;
  }

  vizualize_node(node->left_, dot_file);
  vizualize_node(node->right_, dot_file);
}

template <typename KeyT>
void vizualize_tree(const hwt::SearchTree<KeyT> &tree,
                    const std::string gv_file_name = "tree") {
  if (tree.begin() == tree.end())
    return;

  if (tree.root_->height_ > 7) {  
    std::cout << "Node is too big for vizualization" << std::endl;
    return;
  }

  std::string gv_file_path = gv_file_name + ".gv";
  std::ofstream gv_file;
  gv_file.open(gv_file_path);

  auto left_child = tree.root_->left_;
  auto right_child = tree.root_->right_;

  gv_file << "digraph SearchTree {" << std::endl;

  vizualize_node(tree.root_, gv_file);

  gv_file << "}" << std::endl;

  save_tree_png(gv_file_name);

  gv_file.close();
}