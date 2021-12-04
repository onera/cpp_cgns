#pragma once

#include "cpp_cgns/cgns.hpp"
#include "std_e/future/span.hpp"
#include "std_e/future/contract.hpp"
#include "cpp_cgns/tree_manip.hpp"
#include "cpp_cgns/sids/cgnslib.h"
#include "cpp_cgns/sids/Hierarchical_Structures.hpp"


// SEE http://cgns.github.io/CGNS_docs_current/sids/cgnsbase.html
// From SIDS §7: Grid Coordinates, Elements, and Flow Solutions
namespace cgns {

// NOTE: template parameter Tree is only here to have the const and non-const versions all-at-once (i.e Tree should be substituted by either tree or const tree)

template<class Tree> auto
ElementType_ElementSizeBoundary(Tree& e) -> auto& {
  STD_E_ASSERT(label(e)=="Elements_t");
  return value(e);
}

template<class Tree> auto
ElementType(Tree& e) -> Scalar_ref auto {
  STD_E_ASSERT(label(e)=="Elements_t");
  return ElementType_ElementSizeBoundary(e)(0);
}

template<class Tree> auto
ElementSizeBoundary(Tree& e) -> Scalar_ref auto {
  STD_E_ASSERT(label(e)=="Elements_t");
  return ElementType_ElementSizeBoundary(e)(1);
}

template<class I> auto
is_boundary_partitionned_element_pool(const tree& e) -> bool;

template<class I, class Tree> auto
ElementRange(Tree& e) {
  STD_E_ASSERT(label(e)=="Elements_t");
  return get_child_value_by_name<I>(e,"ElementRange");
}

template<class I, class Tree> auto
ElementConnectivity(Tree& e)  {
  STD_E_ASSERT(label(e)=="Elements_t");
  return get_child_value_by_name<I>(e,"ElementConnectivity");
}

template<class I, class Tree> auto
ParentElements(Tree& e) {
  STD_E_ASSERT(label(e)=="Elements_t");
  return get_child_value_by_name<I,2>(e,"ParentElements");
}

template<class I, class Tree> auto
ElementStartOffset(Tree& e) {
  STD_E_ASSERT(label(e)=="Elements_t");
  return get_child_value_by_name<I>(e,"ElementStartOffset");
}


template<class Tree> auto
element_section(Tree& z, ElementType_t e_type) -> tree& {
  STD_E_ASSERT(label(z)=="Zone_t");
  auto elt_pools = get_children_by_label(z,"Elements_t");
  auto match_elt_type = [e_type](const tree& e_pool){ return ElementType(e_pool)==I4(e_type); };
  auto elt_pool_it = std::find_if(begin(elt_pools),end(elt_pools),match_elt_type);
  STD_E_ASSERT(elt_pool_it!=end(elt_pools));
  return *elt_pool_it;
}

template<class Tree, class F> auto
for_each_unstructured_zone(Tree& b, F f) -> void {
  STD_E_ASSERT(label(b)=="CGNSBase_t");
  auto zs = get_children_by_label(b,"Zone_t");

  for (Tree& z : zs) {
    if (is_unstructured_zone(z)) {
      f(z);
    }
  }
}


} // cgns
