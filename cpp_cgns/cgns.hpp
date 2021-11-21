#pragma once


#include <initializer_list>
#include <iterator>
#include <string>
#include <memory>
#include <functional> // for std::reference_wrapper
#include "cpp_cgns/base/tree.hpp"


namespace cgns {


/// empty node value {
inline auto MT() -> node_value {
  return {"MT",{0},{}};
}
/// empty node value }


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

//inline auto first_child(      tree& t) ->       auto  { return t.children.begin(); }
//inline auto first_child(const tree& t) ->       auto  { return t.children.begin(); }
//inline auto  last_child(      tree& t) ->       auto  { return t.children.end  (); }
//inline auto  last_child(const tree& t) ->       auto  { return t.children.end  (); }
inline auto first_child(      tree& t) ->       tree* { return t.children.data()                    ; }
inline auto first_child(const tree& t) -> const tree* { return t.children.data()                    ; }
inline auto  last_child(      tree& t) ->       tree* { return t.children.data() + t.children.size(); }
inline auto  last_child(const tree& t) -> const tree* { return t.children.data() + t.children.size(); }
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

/// comparison {
//// node_value {
auto same_node_data(const node_value& x, const node_value& y) -> bool;
auto operator==(const node_value& x, const node_value& y) -> bool;
auto operator!=(const node_value& x, const node_value& y) -> bool;
//// node_value }

//// tree {
auto same_tree_structure(const tree& x, const tree& y) -> bool;
auto operator==(const tree& x, const tree& y) -> bool;
auto operator!=(const tree& x, const tree& y) -> bool;
//// tree }
/// comparison }


// utility }


} // cgns
