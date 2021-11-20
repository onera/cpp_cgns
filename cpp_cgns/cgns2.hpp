#pragma once


#include <initializer_list>
#include <iterator>
#include <string>
#include <memory>
#include <functional> // for std::reference_wrapper
#include "std_e/buffer/buffer.hpp"
#include "std_e/buffer/polymorphic_buffer.hpp"
#include "std_e/utils/vector.hpp"
#include "cpp_cgns/exception.hpp"


namespace cgns {


// core {
// from http://cgns.github.io/CGNS_docs_current/filemap/general.html
using C1 = char;
using I4 = std::int32_t;
using I8 = std::int64_t;
using R4 = float;
using R8 = double;


// polymorphic_array allow the CGNS node value
// to hide memory allocation and ownership under a unique type
template<class T> using node_value_underlying_memory = std_e::polymorphic_array<T>;

// variant_range allows the CGNS node value
// to hide its array scalar type under a variant type
using node_value_underlying_range =
  variant_range<
    polymorphic_array
    C1,I4,I8,R4,R8
  >;

using node_value_shape =
  std_e::dyn_shape<I8,std_e::dynamic_size>;

// a CGNS node value is actually a multi-dimensional array...
using node_value_impl =
  std_e::multi_array<
    node_value_underlying_range,
    node_value_shape
  >;


template<class T>
struct is_multi_array_impl {
  static constexpr bool value = false;
};
template<class M0, class M1>
struct is_multi_array_impl<std_e::multi_array<M0,M1>> {
  static constexpr bool value = true;
}
template<class T> constexpr bool is_multi_array = is_multi_array_impl<T>::value;
template<class T> constexpr bool is_1d_array = is_array<T> && !is_multi_array<T>;


// But it can be constructed from various 1D or multi-D arrays
class node_value : public node_value_impl {
  private:
    struct tag_1d {};
    struct tag_multi {};
  public:
    using base = node_value_impl;

    node_value()
      : base(null_array<C1>{},node_value_shape{{}})
    {}

    template<class Array>
      requires
        (   std::is_rvalue_reference_v<Array&&>
         && is_1d_array<Array> )
    node_value(Array&& x)
      : base(x,x.size(),tag_1d{}) // extract size before moving
    {}

    template<class Multi_array>
      requires
        (   std::is_rvalue_reference_v<Multi_array&&>
         && is_multi_array<Multi_array> )
    node_value(Multi_array&& x)
      : base(x,x.extent(),tag_multi{}) // extract extent before moving
    {}

  private:
    template<class Array>
    node_value(Array& x, I8 sz, tag_1d)
      : base(std::move(x),node_value_shape{{sz}})
    {}
    template<class Array, class Multi_index>
    node_value(Array& x, Multi_index&& is, tag_multi)
      : base(std::move(x),node_value_shape{FWD(is)})
    {}
};


struct tree;
class tree_children : public std::deque<tree> {}; // use of std::deque to guarantee reference stability when adding childrens

struct tree {
  auto name ()    -> std::string  &;
  auto label()    -> std::string  &;
  auto value()    -> node_value   &;
  auto children() -> tree_children&;
};




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

inline auto
n_byte(const std::string& dt) -> int {
  if (dt=="C1") return 1;
  if (dt=="I4") return 4;
  if (dt=="I8") return 8;
  if (dt=="R4") return 4;
  if (dt=="R8") return 8;
  else throw cgns_exception("n_byte: unknown data type \"" + dt + "\"");
}
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
