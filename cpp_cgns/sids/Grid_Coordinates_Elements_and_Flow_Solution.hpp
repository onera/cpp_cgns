#pragma once

#include "cpp_cgns/cgns.hpp"
#include "cpp_cgns/node_manip.hpp"
#include "std_e/future/span.hpp"

// SEE http://cgns.github.io/CGNS_docs_current/sids/cgnsbase.html
// From SIDS §7: Grid Coordinates, Elements, and Flow Solutions
namespace cgns {

std_e::span<const I4,2> ElementType_ElementSizeBoundary(const tree& e);
std_e::span<      I4,2> ElementType_ElementSizeBoundary(      tree& e);

I4  ElementType(const tree& e);
I4& ElementType(      tree& e);

I4  ElementSizeBoundary(const tree& e);
I4& ElementSizeBoundary(      tree& e);

bool is_boundary_partitionned_element_pool(const tree& e);

std_e::span<const I4,2> ElementRange(const tree& e);
std_e::span<      I4,2> ElementRange(      tree& e);

std_e::span<const I4> ElementConnectivity(const tree& e);
std_e::span<      I4> ElementConnectivity(      tree& e);

md_array_view<const I4,2> ParentElements(const tree& e);
md_array_view<      I4,2> ParentElements(      tree& e);




template<class I, class Cgns_tree> auto ElementRange2(Cgns_tree&& e) {
  STD_E_ASSERT(e.label=="Elements_t");
  auto& elt_range = get_child_by_name(e,"ElementRange");
  return view_as_span<I,2>(elt_range.value);
}

template<class I, class Cgns_tree> auto ElementConnectivity2(Cgns_tree&& e) {
  STD_E_ASSERT(e.label=="Elements_t");
  auto& elt_conn = get_child_by_name(e,"ElementConnectivity");
  return view_as_span<I>(elt_conn.value);
}

template<class I, class Cgns_tree> auto ParentElements2(Cgns_tree&& e) {
  STD_E_ASSERT(e.label=="Elements_t");
  auto& parent_elts = get_child_by_name(e,"ParentElements");
  return view_as_md_array<I,2>(parent_elts.value);
}

template<class I, class Cgns_tree> auto ElementStartOffset(Cgns_tree&& e) {
  STD_E_ASSERT(e.label=="Elements_t");
  auto& res = get_child_by_name(e,"ElementStartOffset");
  return view_as_md_array<I,2>(res.value);
}

} // cgns
