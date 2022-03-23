#pragma once

#include "cpp_cgns/cgns.hpp"
#include "std_e/future/contract.hpp"
#include "std_e/future/span.hpp"
#include "cpp_cgns/tree_manip.hpp"

// SEE http://cgns.github.io/CGNS_docs_current/sids/cgnsbase.html
// From SIDS §6: Hierarchical Structures
namespace cgns {

// NOTE: template parameter Tree is only here to have the const and non-const versions all-at-once (i.e Tree should be substituted by either tree or const tree)

/// 6.3 Zone Structure Definition: Zone_t
template<class I, class Tree> auto
dims_of_unstruct_zone(Tree& z) {
  STD_E_ASSERT(label(z)=="Zone_t");
  if (!is_unstructured_zone(z))
    throw cgns_exception("dims_of_unstruct_zone expects an unstructured z");
  if (value(z).rank()!=2 || value(z).extent(0)!=1 || value(z).extent(1)!=3)
    throw cgns_exception("CGNS requires unstructured zone dimensions to be an array of shape {1x3}");

  I* zone_dims_ptr = (I*)value(z).data();
  return std_e::make_span<2>(zone_dims_ptr);
}

template<class I, class Tree> auto
VertexSize_U(Tree& z) -> auto& {
  auto zone_dims = dims_of_unstruct_zone<I>(z);
  return zone_dims[0];
}
template<class I, class Tree> auto
CellSize_U(Tree& z) -> auto& {
  auto zone_dims = dims_of_unstruct_zone<I>(z);
  return zone_dims[1];
}
template<class I, class Tree> auto
VertexBoundarySize_U(Tree& z) -> auto& {
  auto zone_dims = dims_of_unstruct_zone<I>(z);
  return zone_dims[2];
}

auto
ZoneType(const tree& z) -> std::string;

auto
is_unstructured_zone(const tree& z) -> bool;

template<class I> auto
is_boundary_partitioned_zone(const tree& z) -> bool;

} // cgns
