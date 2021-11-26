#pragma once


#include <deque>
#include <memory>
#include "cpp_cgns/base/node_value.hpp"
#include <functional> // for std::reference_wrapper // TODO remove


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


/// comparison {
auto same_tree_structure(const tree& x, const tree& y) -> bool;
auto operator==(const tree& x, const tree& y) -> bool;
auto operator!=(const tree& x, const tree& y) -> bool;
/// comparison }


auto to_string(const tree& t, int threshold = default_threshold_to_print_whole_array) -> std::string;


/// access functions {
inline auto name    (      tree& t) ->       std::string  & { return t.name;     }
inline auto name    (const tree& t) -> const std::string  & { return t.name;     }

inline auto label   (      tree& t) ->       std::string  & { return t.label;    }
inline auto label   (const tree& t) -> const std::string  & { return t.label;    }

inline auto value   (      tree& t) ->       node_value   & { return t.value;    }
inline auto value   (const tree& t) -> const node_value   & { return t.value;    }

inline auto children(      tree& t) ->       tree_children& { return t.children; }
inline auto children(const tree& t) -> const tree_children& { return t.children; }
/// access functions }


/// rooted graph interface {
inline auto  first_root(      tree& t) ->       tree* { return  &t   ; }
inline auto  first_root(const tree& t) -> const tree* { return  &t   ; }
inline auto   last_root(      tree& t) ->       tree* { return (&t)+1; }
inline auto   last_root(const tree& t) -> const tree* { return (&t)+1; }

inline auto first_child(      tree& t) -> auto { return t.children.begin(); }
inline auto first_child(const tree& t) -> auto { return t.children.begin(); }
inline auto  last_child(      tree& t) -> auto { return t.children.end()  ; }
inline auto  last_child(const tree& t) -> auto { return t.children.end()  ; }
/// rooted graph interface }


/// range {
/// TODO write proper range without using ref_wrapper (too complicated)
template<class Tree> using ref = std::reference_wrapper<Tree>;
template<class Tree> using range = std::vector<Tree>;
template<class Tree> using range_of_ref = range<ref<Tree>>;

using tree_ref       = ref<      tree>;
using const_tree_ref = ref<const tree>;

using tree_range       = range_of_ref<      tree>;
using const_tree_range = range_of_ref<const tree>;
/// range }


/// children {
inline auto
emplace_child(tree& t, tree&& c) -> tree& {
  t.children.emplace_back(std::move(c));
  return t.children.back();
}

inline auto
emplace_children(tree& t, std::vector<tree>&& cs) -> void {
  for (size_t i=0; i<cs.size(); ++i) {
    emplace_child(t,std::move(cs[i]));
  }
}
/// children }


} // cgns
