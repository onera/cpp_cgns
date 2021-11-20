#pragma once


#include <deque>
#include <memory>
#include "cpp_cgns/base/node_value.hpp"


namespace cgns {


class tree;

// use of std::deque to guarantee reference stability when adding childrens
class tree_children : public std::deque<tree> {};


namespace detail {

struct internal_tree_base {
  virtual auto name ()          ->       std::string  & = 0;
  virtual auto name ()    const -> const std::string  & = 0;
  virtual auto label()          ->       std::string  & = 0;
  virtual auto label()    const -> const std::string  & = 0;
  virtual auto value()          ->       node_value   & = 0;
  virtual auto value()    const -> const node_value   & = 0;
  virtual auto children()       ->       tree_children& = 0;
  virtual auto children() const -> const tree_children& = 0;

  virtual ~internal_tree_base() {}
};

template<class Tree>
struct internal_tree_impl: internal_tree_base {
  public:
    internal_tree_impl(Tree t)
      : t(std::move(t))
    {}

  private:
    Tree t;
};

} // detail


template<class T>
class tree {
  public:
    tree()
      : impl(nullptr)
    {}

    template<class Tree>
      requires (!std::is_same_v<tree, std::decay_t<Tree>)
    tree(Tree&& x):
      impl(std::make_unique<detail::internal_tree_impl<Tree>>(std::move(x)))
    {}

  private:
    std::unique_ptr<detail::internal_tree_base<T>> impl;
};


} // cgns
