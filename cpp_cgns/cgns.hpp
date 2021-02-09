#pragma once


#include <initializer_list>
#include <iterator>
#include <string>
#include <memory>
#include <functional> // for std::reference_wrapper
#include "std_e/buffer/buffer.hpp"
#include "std_e/buffer/polymorphic_buffer.hpp"
#include "std_e/utils/vector.hpp"


namespace cgns {


// core {
// from http://cgns.github.io/CGNS_docs_current/filemap/general.html
using C1 = char;
using I4 = std::int32_t;
using I8 = std::int64_t;
using R4 = float;
using R8 = double;


struct node_value {
  std::string data_type; // TODO enum
  std::vector<I8> dims;
  std_e::polymorphic_buffer buffer;

  node_value() = default;
  node_value(node_value&&) = default;
  node_value& operator=(node_value&&) = default;
  node_value(const node_value&) = delete;
  node_value& operator=(const node_value&) = delete;
};
inline auto
data(node_value& x) -> void* {
  return x.buffer.data();
}
inline auto
data(const node_value& x) -> const void* {
  return x.buffer.data();
}

struct tree {
  std::string name;
  std::string label;
  node_value value;
  std::vector<tree> children;


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
inline auto MT() -> node_value {
  return {"MT",{0},{}};
}
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
