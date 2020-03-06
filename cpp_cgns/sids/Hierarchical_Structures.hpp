#pragma once

#include "cpp_cgns/cpp_cgns.hpp"
#include <array>

// SEE http://cgns.github.io/CGNS_docs_current/sids/cgnsbase.html
// From SIDS §6: Hierarchical Structures
namespace cpp_cgns {

/// 6.3 Zone Structure Definition: Zone_t
std::array<I4,3> dims_of_unstruct_zone(const tree& zone);
I4 VertexSize_U(const tree& zone);
I4 CellSize_U(const tree& zone);
I4 VertexBoundarySize_U(const tree& zone);

bool is_boundary_partitionned_zone(const tree& z);

} // cpp_cgns
