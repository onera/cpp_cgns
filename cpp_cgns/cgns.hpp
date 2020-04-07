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

using g_idx_t = I8;
using l_idx_t = I4;


struct node_value {
  std::string data_type;
  std::vector<I8> dims;
  void* data;
};

struct tree {
  std::string name;
  node_value value;
  std::vector<tree> children;
  std::string label;
};
// core }


// utility {

/// basic type to_string {
template<class T> std::string to_string()     { return "Unknown CGNS data_type"; };
template<> inline std::string to_string<C1>() { return "C1"; }
template<> inline std::string to_string<I4>() { return "I4"; }
template<> inline std::string to_string<I8>() { return "I8"; }
template<> inline std::string to_string<R4>() { return "R4"; }
template<> inline std::string to_string<R8>() { return "R8"; }
template<> inline std::string to_string<const C1>() { return "C1"; }
template<> inline std::string to_string<const I4>() { return "I4"; }
template<> inline std::string to_string<const I8>() { return "I8"; }
template<> inline std::string to_string<const R4>() { return "R4"; }
template<> inline std::string to_string<const R8>() { return "R8"; }
/// basic type to_string }


/// empty node {
const node_value MT = {"MT",{0},nullptr};
/// empty node }


/// access functions {
inline auto name    (      tree& t) ->       std::string      & { return t.name;     }
inline auto name    (const tree& t) -> const std::string      & { return t.name;     }

inline auto value   (      tree& t) ->       node_value       & { return t.value;    }
inline auto value   (const tree& t) -> const node_value       & { return t.value;    }

inline auto children(      tree& t) ->       std::vector<tree>& { return t.children; }
inline auto children(const tree& t) -> const std::vector<tree>& { return t.children; }

inline auto label   (      tree& t) ->       std::string      & { return t.label;    }
inline auto label   (const tree& t) -> const std::string      & { return t.label;    }
/// access functions }


/// range {
using tree_ref = std::reference_wrapper<tree>;
using tree_range = std::vector<tree_ref>;

using const_tree_ref = std::reference_wrapper<const tree>;
using const_tree_range = std::vector<const_tree_ref>;
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

// utility }


} // cgns
