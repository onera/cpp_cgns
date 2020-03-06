#pragma once

#include "cpp_cgns/cpp_cgns.hpp"
#include <array>

// SEE http://cgns.github.io/CGNS_docs_current/sids/cgnsbase.html
// From SIDS §7: Grid Coordinates, Elements, and Flow Solutions
namespace cpp_cgns {

std::array<I4,2> ElementType_ElementSizeBoundary(const tree& e);
I4 ElementType(const tree& e);
I4 ElementSizeBoundary(const tree& e);
bool is_boundary_partitionned_element_pool(const tree& e);

} // cpp_cgns
