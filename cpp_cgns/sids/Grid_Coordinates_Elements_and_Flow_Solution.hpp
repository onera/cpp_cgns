#pragma once

#include "cpp_cgns/cgns.hpp"
#include "cpp_cgns/node_manip.hpp"
#include "std_e/future/span.hpp"
#include "std_e/future/contract.hpp"
#include "cpp_cgns/exception.hpp"
#include "cpp_cgns/tree_manip.hpp"

// SEE http://cgns.github.io/CGNS_docs_current/sids/cgnsbase.html
// From SIDS §7: Grid Coordinates, Elements, and Flow Solutions
namespace cgns {

// NOTE: template parameter Tree is only here to have the const and non-const versions all-at-once (i.e Tree should be substituted by either tree or const tree)

template<class I, class Tree> auto
ElementType_ElementSizeBoundary(Tree& e) {
  STD_E_ASSERT(label(e)=="Elements_t");
  if (value(e).data_type!="I4")
    throw cgns_exception("CGNS requires zone dimensions to be of type \"I4\""); // TODO I8 (and ssqq)
  I* e_ptr = (I*)value(e).data;
  return std_e::make_span<2>(e_ptr);
}

template<class I, class Tree> auto
ElementType(Tree& e) -> I& {
  STD_E_ASSERT(label(e)=="Elements_t");
  return ElementType_ElementSizeBoundary<I>(e)[0];
}

template<class I, class Tree> auto
ElementSizeBoundary(Tree& e) -> I& {
  STD_E_ASSERT(label(e)=="Elements_t");
  return ElementType_ElementSizeBoundary<I>(e)[1];
}

bool is_boundary_partitionned_element_pool(const tree& e);

template<class I, class Tree> auto 
ElementRange(Tree& e) {
  STD_E_ASSERT(label(e)=="Elements_t");
  auto& elt_range = get_child_by_name(e,"ElementRange");
  return view_as_span<I,2>(elt_range.value);
}

template<class I, class Tree> auto
ElementConnectivity(Tree& e) {
  STD_E_ASSERT(label(e)=="Elements_t");
  auto& elt_conn = get_child_by_name(e,"ElementConnectivity");
  return view_as_span<I>(elt_conn.value);
}

template<class I, class Tree> auto
ParentElements(Tree& e) {
  STD_E_ASSERT(label(e)=="Elements_t");
  auto& parent_elts = get_child_by_name(e,"ParentElements");
  return view_as_md_array<I,2>(parent_elts.value);
}

template<class I, class Tree> auto
ElementStartOffset(Tree& e) {
  STD_E_ASSERT(label(e)=="Elements_t");
  auto& res = get_child_by_name(e,"ElementStartOffset");
  return view_as_md_array<I,2>(res.value);
}

} // cgns
