#pragma once

#include "cpp_cgns/cgns.hpp"
#include "std_e/future/contract.hpp"
#include "cpp_cgns/node_manip.hpp"
#include "cpp_cgns/tree_manip.hpp"
#include "cpp_cgns/exception.hpp"

// SEE http://cgns.github.io/CGNS_docs_current/sids/cgnsbase.html
// From SIDS §6: Hierarchical Structures
namespace cgns {

// NOTE: template parameter Tree is only here to have the const and non-const versions all-at-once (i.e Tree should be substituted by either tree or const tree)

/// 6.3 Zone Structure Definition: Zone_t
template<class I, class Tree> auto dims_of_unstruct_zone(Tree& z) {
  STD_E_ASSERT(z.label=="Zone_t");
  if (!is_unstructured_zone(z))
    throw cgns_exception("dims_of_unstruct_zone expects an unstructured z");
  if (z.value.dims.size()!=2 || z.value.dims[0]!=1 || z.value.dims[1]!=3)
    throw cgns_exception("CGNS requires unstructured z dimensions to be an array of shape {1x3}");

  I* zone_dims_ptr = (I*)z.value.data;
  return std_e::make_span<2>(zone_dims_ptr);
}

template<class I, class Tree> I& VertexSize_U(Tree& z) {
  auto zone_dims = dims_of_unstruct_zone<I>(z);
  return zone_dims[0];
}
template<class I, class Tree> I& CellSize_U(Tree& z) {
  auto zone_dims = dims_of_unstruct_zone<I>(z);
  return zone_dims[1];
}
template<class I, class Tree> I& VertexBoundarySize_U(Tree& z) {
  auto zone_dims = dims_of_unstruct_zone<I>(z);
  return zone_dims[2];
}

bool is_unstructured_zone(const tree& z);
bool is_boundary_partitionned_zone(const tree& z);

} // cgns
