#include "cpp_cgns/sids/Hierarchical_Structures.hpp"

namespace cgns {

auto
is_unstructured_zone(const tree& z) -> bool {
  STD_E_ASSERT(label(z)=="Zone_t");
  const tree& zone_type = get_child_by_label(z,"ZoneType_t");
  return to_string(value(zone_type))=="Unstructured";
}

template<class I> auto
is_boundary_partitionned_zone(const tree& z) -> bool {
  STD_E_ASSERT(label(z)=="Zone_t");
  return VertexBoundarySize_U<I>(z) != 0; // per CGNS SIDS 6.3
}

// explicit instanciations (do not pollute the header for only 2 instanciations)
template auto is_boundary_partitionned_zone<I4>(const tree& z) -> bool;
template auto is_boundary_partitionned_zone<I8>(const tree& z) -> bool;

} // cgns
