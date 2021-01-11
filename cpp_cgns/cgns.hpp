#pragma once


#include <string>
#include <vector>
#include <functional> // for std::reference_wrapper


namespace cgns {


// core {
// from http://cgns.github.io/CGNS_docs_current/filemap/general.html
using C1 = char;
using I4 = std::int32_t;
using I8 = std::int64_t;
using R4 = float;
using R8 = double;


struct node_value {
  std::string data_type;
  std::vector<I8> dims;
  void* data;
};

struct tree {
  std::string name;
  std::string label;
  node_value value;
  std::vector<tree> children;
};
// core }


// utility {

/// basic type to_string {
template<class T> auto to_string          () -> std::string { return "Unknown CGNS data_type"; };
template<> inline auto to_string<      C1>() -> std::string { return "C1"; }
template<> inline auto to_string<const C1>() -> std::string { return "C1"; }
template<> inline auto to_string<      I4>() -> std::string { return "I4"; }
template<> inline auto to_string<const I4>() -> std::string { return "I4"; }
template<> inline auto to_string<      I8>() -> std::string { return "I8"; }
template<> inline auto to_string<const I8>() -> std::string { return "I8"; }
template<> inline auto to_string<      R4>() -> std::string { return "R4"; }
template<> inline auto to_string<const R4>() -> std::string { return "R4"; }
template<> inline auto to_string<      R8>() -> std::string { return "R8"; }
template<> inline auto to_string<const R8>() -> std::string { return "R8"; }
/// basic type to_string }


/// empty node value {
const node_value MT = {"MT",{0},nullptr};
/// empty node value }


/// access functions {
inline auto name    (      tree& t) ->       std::string      & { return t.name;     }
inline auto name    (const tree& t) -> const std::string      & { return t.name;     }

inline auto label   (      tree& t) ->       std::string      & { return t.label;    }
inline auto label   (const tree& t) -> const std::string      & { return t.label;    }

inline auto value   (      tree& t) ->       node_value       & { return t.value;    }
inline auto value   (const tree& t) -> const node_value       & { return t.value;    }

inline auto children(      tree& t) ->       std::vector<tree>& { return t.children; }
inline auto children(const tree& t) -> const std::vector<tree>& { return t.children; }
/// access functions }


/// rooted graph interface {
inline auto  first_root(      tree& t) ->       tree* { return  &t   ; }
inline auto  first_root(const tree& t) -> const tree* { return  &t   ; }
inline auto   last_root(      tree& t) ->       tree* { return (&t)+1; }
inline auto   last_root(const tree& t) -> const tree* { return (&t)+1; }

inline auto first_child(      tree& t) ->       tree* { return t.children.data()                    ; }
inline auto first_child(const tree& t) -> const tree* { return t.children.data()                    ; }
inline auto  last_child(      tree& t) ->       tree* { return t.children.data() + t.children.size(); }
inline auto  last_child(const tree& t) -> const tree* { return t.children.data() + t.children.size(); }
/// rooted graph interface }


/// range {
template<class Tree> using ref = std::reference_wrapper<Tree>;
template<class Tree> using range = std::vector<Tree>;
template<class Tree> using range_of_ref = range<ref<Tree>>;

using tree_ref       = ref<      tree>;
using const_tree_ref = ref<const tree>;

using tree_range       = range_of_ref<      tree>;
using const_tree_range = range_of_ref<const tree>;
/// range }


/// children {
inline 
tree& emplace_child(tree& t, tree&& c) {
  t.children.emplace_back(std::move(c));
  return t.children.back();
}

inline
void emplace_children(tree& t, std::vector<tree>&& cs) {
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
