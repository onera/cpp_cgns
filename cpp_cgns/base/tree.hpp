#pragma once


#include <deque>
#include <memory>
#include "cpp_cgns/base/node_value.hpp"


namespace cgns {


class tree;

// TODO use of std::deque to guarantee reference stability when adding childrens
//class tree_children : public std::deque<tree> {};
class tree_children : public std::vector<tree> {};



struct tree {
  std::string name;
  std::string label;
  node_value value;
  tree_children children;


  tree() = default;
  tree(tree&&) = default;
  tree& operator=(tree&&) = default;
  tree(const tree&) = delete;
  tree& operator=(const tree&) = delete;

  tree(std::string name, std::string label, node_value value)
    : name(std::move(name))
    , label(std::move(label))
    , value(std::move(value))
  {}

  template<class Tree>
  tree(std::string name, std::string label, node_value value, std::vector<Tree> children)
    : name(std::move(name))
    , label(std::move(label))
    , value(std::move(value))
    , children(std::move(children))
  {}

  tree(std::string name, std::string label, node_value value, std::initializer_list<tree> children)
    : name(std::move(name))
    , label(std::move(label))
    , value(std::move(value))
    , children(std_e::move_to_vector((tree*)children.begin(),(tree*)children.end())) // need this to overcome std::init_list and std::move limitations
  {}
};



} // cgns
