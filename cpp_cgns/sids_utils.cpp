#include "cpp_cgns/cgns_exception.hpp"
#include "cpp_cgns/sids_utils.hpp"
#include "cpp_cgns/sids_manip.hpp"
#include "cpp_cgns/elements_utils.hpp"
#include "std_e/multi_array/utils.hpp"


namespace cpp_cgns {


std::array<I4,3> dims_of_unstruct_zone(tree& zone) {
  STD_E_ASSERT(zone.type=="Zone_t");
  tree& zone_type = get_child_by_type(zone,"ZoneType_t");
  if (zone.type!="Zone_t") 
    throw cgns_exception("dims_of_unstruct_zone expects a Zone_t tree");
  if (to_string(zone_type.value)!="Unstructured")
    throw cgns_exception("dims_of_unstruct_zone expects an unstructured zone");
  if (zone.value.data_type!="I4")
    throw cgns_exception("CGNS requires zone dimensions to be of type \"I4\"");
  if (zone.value.dims.size()!=2 || zone.value.dims[0]!=1 || zone.value.dims[1]!=3)
    throw cgns_exception("CGNS requires unstructured zone dimensions to be an array of shape {1x3}");

  I4* zone_dims_ptr = (I4*)zone.value.data;
  return {zone_dims_ptr[0],zone_dims_ptr[1],zone_dims_ptr[2]};
}

I4 number_of_nodes_of_unstruct_zone(tree& zone) {
  std::array<I4,3> zone_dims = dims_of_unstruct_zone(zone);
  return zone_dims[0];
}

void correct_bc_data_arrays_according_to_SIDS(tree& bcdata_node) {
  // SIDS: boundary field should have shape {1,PointList size}
  tree_range bcdata_array_nodes = get_children_by_type(bcdata_node,"DataArray_t");
  for (tree& bcdata_array_node : bcdata_array_nodes) {
    bcdata_array_node.value.dims = {1,bcdata_array_node.value.dims[0]};
  }
}


CGNS_ENUMT(ElementType_t) element_type(tree& elements_node) {
  STD_E_ASSERT(elements_node.type=="Elements_t");
  auto val = view_as_span<I4>(elements_node.value);
  return (CGNS_ENUMT(ElementType_t))val[0]; // from SIDS File Mapping Manual, Elements_t
}
std_e::integer_closed_range_ref<I4> element_range(tree& elements_node) {
  STD_E_ASSERT(elements_node.type=="Elements_t");
  tree& elt_range_node = get_child_by_name(elements_node, "ElementRange");
  auto elt_range = view_as_span<I4>(elt_range_node.value);
  return {elt_range[0],elt_range[1]};
}
I8 nb_of_elements(tree& elements_node) {
  STD_E_ASSERT(elements_node.type=="Elements_t");
  return length(element_range(elements_node));
}

md_array_view<I4,2> regular_elements_connectivities(tree& elements_node) {
  STD_E_ASSERT(elements_node.type=="Elements_t");
  ElementType_t elt_type = element_type(elements_node);
  I8 nb_nodes_for_elt_type = number_of_nodes(elt_type);
  I8 nb_elts = nb_of_elements(elements_node);
  tree& element_connectivity_node = get_child_by_name(elements_node, "ElementConnectivity");

  node_value elt_conn_val = element_connectivity_node.value;
  elt_conn_val.dims = {nb_nodes_for_elt_type,nb_elts};
  return view_as_md_array<I4,2>(elt_conn_val);;
}


bool compare_by_range(tree& elts_node0, tree& elts_node1) {
  STD_E_ASSERT(elts_node0.type=="Elements_t");
  STD_E_ASSERT(elts_node1.type=="Elements_t");
  return element_range(elts_node0).first < element_range(elts_node1).first;
}
bool equal_by_elt_type(tree& elts_node0, tree& elts_node1) {
  STD_E_ASSERT(elts_node0.type=="Elements_t");
  STD_E_ASSERT(elts_node1.type=="Elements_t");
  return element_type(elts_node0) == element_type(elts_node1);
}
bool compare_by_elt_type(tree& elts_node0, tree& elts_node1) {
  STD_E_ASSERT(elts_node0.type=="Elements_t");
  STD_E_ASSERT(elts_node1.type=="Elements_t");
  return element_type(elts_node0) < element_type(elts_node1);
}


} // cpp_cgns
