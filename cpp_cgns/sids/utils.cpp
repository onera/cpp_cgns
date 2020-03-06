#include "cpp_cgns/cgns_exception.hpp"
#include "cpp_cgns/sids/utils.hpp"
#include "cpp_cgns/tree_manip.hpp"
#include "cpp_cgns/sids/elements_utils.hpp"
#include "std_e/multi_array/utils.hpp"
#include "cpp_cgns/cgns_exception.hpp"


namespace cpp_cgns {


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
bool is_boundary(const node_value& parent_elts, int i) {
  if (parent_elts.data_type=="I4") {
    auto pe = view_as_md_array<I4,2>(parent_elts);
    if ((pe(i,0)==0) || (pe(i,1)==0)) { // 0 means no parent element, as per CGNS SIDS 7.3
      return true;
    }
    return false;
  } else if (parent_elts.data_type=="I8") {
    auto pe = view_as_md_array<I8,2>(parent_elts);
    if ((pe(i,0)==0) || (pe(i,1)==0)) { // 0 means no parent element, as per CGNS SIDS 7.3
      return true;
    }
    return false;
  } else {
    throw cgns_exception("ParentElements node must be of type I4 or I8, not " + parent_elts.data_type);
  }
}


} // cpp_cgns