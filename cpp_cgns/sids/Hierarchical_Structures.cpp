#include "cpp_cgns/sids/Hierarchical_Structures.hpp"
#include "std_e/future/contract.hpp"
#include "cpp_cgns/tree_manip.hpp"
#include "cpp_cgns/cgns_exception.hpp"


namespace cpp_cgns {


std::array<I4,3> dims_of_unstruct_zone(const tree& zone) {
  STD_E_ASSERT(zone.type=="Zone_t");
  const tree& zone_type = get_child_by_type(zone,"ZoneType_t");
  if (zone.type!="Zone_t") 
    throw cgns_exception("dims_of_unstruct_zone expects a Zone_t tree");
  if (to_string(zone_type.value)!="Unstructured")
    throw cgns_exception("dims_of_unstruct_zone expects an unstructured zone");
  if (zone.value.data_type!="I4")
    throw cgns_exception("CGNS requires zone dimensions to be of type \"I4\""); // TODO I8 (and ssqq)
  if (zone.value.dims.size()!=2 || zone.value.dims[0]!=1 || zone.value.dims[1]!=3)
    throw cgns_exception("CGNS requires unstructured zone dimensions to be an array of shape {1x3}");

  I4* zone_dims_ptr = (I4*)zone.value.data;
  return {zone_dims_ptr[0],zone_dims_ptr[1],zone_dims_ptr[2]};
}

I4 VertexSize_U(const tree& zone) {
  std::array<I4,3> zone_dims = dims_of_unstruct_zone(zone);
  return zone_dims[0];
}
I4 CellSize_U(const tree& zone) {
  std::array<I4,3> zone_dims = dims_of_unstruct_zone(zone);
  return zone_dims[1];
}
I4 VertexBoundarySize_U(const tree& zone) {
  std::array<I4,3> zone_dims = dims_of_unstruct_zone(zone);
  return zone_dims[2];
}

bool is_boundary_partitionned_zone(const tree& z) {
  STD_E_ASSERT(z.type=="Zone_t");
  return VertexBoundarySize_U(z) != 0; // per CGNS SIDS 6.3
}


} // cpp_cgns
