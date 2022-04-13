#pragma once


#include <deque>
#include "cpp_cgns/base/node_value.hpp"
#include "std_e/meta/pack.hpp"
#include <functional> // for std::reference_wrapper


namespace cgns {


class tree;
class tree_children;


// [Sphinx Doc] tree access {
auto name    (      tree& t) ->       std::string  &;
auto name    (const tree& t) -> const std::string  &;

auto label   (      tree& t) ->       std::string  &;
auto label   (const tree& t) -> const std::string  &;

auto value   (      tree& t) ->       node_value   &;
auto value   (const tree& t) -> const node_value   &;

auto children(      tree& t) ->       tree_children&;
auto children(const tree& t) -> const tree_children&;
// [Sphinx Doc] tree access }


// [Sphinx Doc] tree children {
auto emplace_child(tree& t, tree&& c) -> tree&;
auto emplace_children(tree& t, std::vector<tree>&& cs) -> void;

auto number_of_children(const tree& t) -> int;
template<class Tree> auto child(Tree& t, int i) -> auto&;
// [Sphinx Doc] tree children }


// ====================== impl ======================
class tree_children : public std::deque<tree> {
  public:
    using base = std::deque<tree>; // std::deque to guarantee reference stability when adding childrens

  // ctors
    /// special
    tree_children() = default;
    tree_children(tree_children&&) = default;
    tree_children& operator=(tree_children&&) = default;
    tree_children(const tree_children&) = delete;
    tree_children& operator=(const tree_children&) = delete;

    /// from size
    tree_children(int n)
      : base(n)
    {}

    /// from range
    tree_children(base&& v);

    /// from init list
    template<class... Trees>
      requires (std_e::are_all_of<tree,Trees...>)
    tree_children(Trees&&... ts);
};




class tree {
  private:
    std::string name_;
    std::string label_;
    node_value value_;
    tree_children children_;
  public:
  // ctors
    // special
    tree() = default;
    tree(tree&&) = default;
    tree& operator=(tree&&) = default;

    tree(const tree&) = delete;
    tree& operator=(const tree&) = delete;

    // with number of children
    template<std::integral I>
    tree(std::string name, std::string label, node_value value, I number_of_children)
      : name_(std::move(name))
      , label_(std::move(label))
      , value_(std::move(value))
      , children_(number_of_children)
    {}

    // with no child
    tree(std::string name, std::string label, node_value value)
      : tree(std::move(name),std::move(label),std::move(value),0)
    {}

    // with range of children
    template<class Tree_range>
      requires (!std::integral<Tree_range> && std::is_rvalue_reference_v<Tree_range&&>)
    tree(std::string name, std::string label, node_value value, Tree_range&& children)
      : tree(std::move(name),std::move(label),std::move(value),children.size())
    {
      std::move(children.begin(),children.end(),begin(children_));
    }

    // with range of children, specialized for init-list
    tree(std::string name, std::string label, node_value value, std::initializer_list<tree> children)
      : tree(std::move(name),std::move(label),std::move(value),children.size())
    {
      std::move((tree*)children.begin(),(tree*)children.end(),begin(children_));
    }

  // access functions
    // NOTE: access functions are non-member because we want them to also work on std::reference_wrapper<Tree>
    friend inline auto name    (      tree& t) ->       std::string  & { return t.name_;     }
    friend inline auto name    (const tree& t) -> const std::string  & { return t.name_;     }

    friend inline auto label   (      tree& t) ->       std::string  & { return t.label_;    }
    friend inline auto label   (const tree& t) -> const std::string  & { return t.label_;    }

    friend inline auto value   (      tree& t) ->       node_value   & { return t.value_;    }
    friend inline auto value   (const tree& t) -> const node_value   & { return t.value_;    }

    friend inline auto children(      tree& t) ->       tree_children& { return t.children_; }
    friend inline auto children(const tree& t) -> const tree_children& { return t.children_; }
};


// comparison {
auto same_tree_structure(const tree& x, const tree& y) -> bool;
auto operator==(const tree& x, const tree& y) -> bool;
auto operator!=(const tree& x, const tree& y) -> bool;
// comparison }


auto to_string(const tree& t, int threshold = default_threshold_to_print_whole_array) -> std::string;


// rooted graph interface {
inline auto  first_root(      tree& t) ->       tree* { return  &t   ; }
inline auto  first_root(const tree& t) -> const tree* { return  &t   ; }
inline auto   last_root(      tree& t) ->       tree* { return (&t)+1; }
inline auto   last_root(const tree& t) -> const tree* { return (&t)+1; }

inline auto first_child(      tree& t) -> auto { return children(t).begin(); }
inline auto first_child(const tree& t) -> auto { return children(t).begin(); }
inline auto  last_child(      tree& t) -> auto { return children(t).end()  ; }
inline auto  last_child(const tree& t) -> auto { return children(t).end()  ; }
// rooted graph interface }


// children {
inline tree_children::
tree_children(base&& v)
  : base(std::move(v))
{}
template<class... Trees>
  requires (std_e::are_all_of<tree,Trees...>)
    tree_children::
tree_children(Trees&&... ts)
{
  ( this->emplace_back(std::move(ts)) , ... );
}


inline auto
emplace_child(tree& t, tree&& c) -> tree& {
  return children(t).emplace_back(std::move(c));
}

inline auto
emplace_children(tree& t, std::vector<tree>&& cs) -> void {
  for (size_t i=0; i<cs.size(); ++i) {
    emplace_child(t,std::move(cs[i]));
  }
}

inline auto
number_of_children(const tree& t) -> int {
  return children(t).size();
}
template<class Tree> auto
child(Tree& t, int i) -> auto& {
  return children(t)[i];
}
// children }


// range {
template<class Tree> using Tree_range = std::vector<std::reference_wrapper<Tree>>;

using tree_range       = Tree_range<      tree>;
using const_tree_range = Tree_range<const tree>;
// range }


// convert std::reference_wrapper<Tree> to Tree& {
template<class Tree> struct tree_ref_impl;

/// non-const
template<> struct tree_ref_impl<                                   tree > { using type =       tree&; };
template<> struct tree_ref_impl<      std::reference_wrapper<      tree>> { using type =       tree&; };
template<> struct tree_ref_impl<const std::reference_wrapper<      tree>> { using type =       tree&; };
/// const
template<> struct tree_ref_impl<                             const tree > { using type = const tree&; };
template<> struct tree_ref_impl<      std::reference_wrapper<const tree>> { using type = const tree&; };
template<> struct tree_ref_impl<const std::reference_wrapper<const tree>> { using type = const tree&; };

template<class Tree> using tree_ref = typename tree_ref_impl<std::remove_reference_t<Tree>>::type;
// convert std::reference_wrapper<Tree> to Tree& }


} // cgns
