#include "cpp_cgns/sids/Hierarchical_Structures.hpp"

namespace cgns {

bool is_unstructured_zone(const tree& z) {
  STD_E_ASSERT(z.label=="Zone_t");
  const tree& zone_type = get_child_by_label(z,"ZoneType_t");
  return to_string(zone_type.value)=="Unstructured";
}
bool is_boundary_partitionned_zone(const tree& z) {
  STD_E_ASSERT(z.label=="Zone_t");
  return VertexBoundarySize_U<I4>(z) != 0; // per CGNS SIDS 6.3 // TODO I4 -> I4/I8
}

} // cgns
