#pragma once

#include "cpp_cgns/cpp_cgns.hpp"
#include "cpp_cgns/cpp_cgns_utils.hpp"
#include "std_e/future/span.hpp"

// SEE http://cgns.github.io/CGNS_docs_current/sids/cgnsbase.html
// From SIDS §7: Grid Coordinates, Elements, and Flow Solutions
namespace cpp_cgns {

std_e::span<const I4,2> ElementType_ElementSizeBoundary(const tree& e);
std_e::span<I4,2> ElementType_ElementSizeBoundary(tree& e);

I4 ElementType(const tree& e);
I4& ElementType(tree& e);

I4 ElementSizeBoundary(const tree& e);
I4& ElementSizeBoundary(tree& e);

bool is_boundary_partitionned_element_pool(const tree& e);

std_e::span<const I4,2> ElementRange(const tree& e);
std_e::span<I4,2> ElementRange(tree& e);

std_e::span<const I4> ElementConnectivity(const tree& e);
std_e::span<I4> ElementConnectivity(tree& e);

md_array_view<const I4,2> ParentElements(const tree& e);
md_array_view<I4,2> ParentElements(tree& e);

} // cpp_cgns
