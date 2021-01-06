#include "cpp_cgns/exception.hpp"
#include "cpp_cgns/sids/utils.hpp"
#include "cpp_cgns/tree_manip.hpp"
#include "cpp_cgns/sids/elements_utils.hpp"
#include "std_e/multi_array/utils.hpp"


namespace cgns {


auto
correct_bc_data_arrays_according_to_SIDS(tree& bcdata_node) -> void {
  // SIDS: boundary field should have shape {1,PointList size}
  tree_range bcdata_array_nodes = get_children_by_label(bcdata_node,"DataArray_t");
  for (tree& bcdata_array_node : bcdata_array_nodes) {
    bcdata_array_node.value.dims = {1,bcdata_array_node.value.dims[0]};
  }
}

auto
nb_of_elements(const tree& elements_node) -> I8 {
  STD_E_ASSERT(elements_node.label=="Elements_t");
  return length(element_range(elements_node));
}


// TODO dispatch by type
auto
element_type(const tree& elements_node) -> ElementType_t {
  STD_E_ASSERT(elements_node.label=="Elements_t");
  if (elements_node.value.data_type=="I4") {
    auto val = view_as_span<I4>(elements_node.value);
    return (ElementType_t)val[0]; // from SIDS File Mapping Manual, Elements_t
  } else if (elements_node.value.data_type=="I8") {
    auto val = view_as_span<I8>(elements_node.value);
    return (ElementType_t)val[0]; // from SIDS File Mapping Manual, Elements_t
  } else {
    throw cgns_exception("Elements_t value must be an integer");
  }
}
auto
element_range(const tree& elements_node) -> std_e::integer_closed_range<I8> {
  STD_E_ASSERT(elements_node.label=="Elements_t");
  const tree& elt_range_node = get_child_by_name(elements_node, "ElementRange");
  if (elt_range_node.value.data_type=="I4") {
    auto elt_range = view_as_span<I4>(elt_range_node.value);
    return {elt_range[0],elt_range[1]};
  } else if (elements_node.value.data_type=="I8") {
    auto elt_range = view_as_span<I8>(elt_range_node.value);
    return {elt_range[0],elt_range[1]};
  } else {
    throw cgns_exception("Elements_t value must be an integer");
  }
}

template<class I> auto
element_range(tree& elements_node) -> std_e::integer_closed_range_ref<I> {
  STD_E_ASSERT(elements_node.label=="Elements_t");
  tree& elt_range_node = get_child_by_name(elements_node, "ElementRange");
  auto elt_range = view_as_span<I>(elt_range_node.value);
  return {elt_range[0],elt_range[1]};
}
template<class I> auto
regular_elements_connectivities(tree& elements_node) -> md_array_view<I,2> {
  STD_E_ASSERT(elements_node.label=="Elements_t");
  ElementType_t elt_type = element_type(elements_node);
  I8 nb_nodes_for_elt_type = number_of_nodes(elt_type);
  I8 nb_elts = nb_of_elements(elements_node);
  tree& element_connectivity_node = get_child_by_name(elements_node, "ElementConnectivity");

  node_value elt_conn_val = element_connectivity_node.value;
  elt_conn_val.dims = {nb_nodes_for_elt_type,nb_elts};
  return view_as_md_array<I,2>(elt_conn_val);;
}


auto
compare_by_range(const tree& elts_node0, const tree& elts_node1) -> bool {
  STD_E_ASSERT(elts_node0.label=="Elements_t");
  STD_E_ASSERT(elts_node1.label=="Elements_t");
  return element_range(elts_node0).first < element_range(elts_node1).first;
}
auto
equal_by_elt_type(const tree& elts_node0, const tree& elts_node1) -> bool {
  STD_E_ASSERT(elts_node0.label=="Elements_t");
  STD_E_ASSERT(elts_node1.label=="Elements_t");
  return element_type(elts_node0) == element_type(elts_node1);
}
auto
compare_by_elt_type(const tree& elts_node0, const tree& elts_node1) -> bool {
  STD_E_ASSERT(elts_node0.label=="Elements_t");
  STD_E_ASSERT(elts_node1.label=="Elements_t");
  return element_type(elts_node0) < element_type(elts_node1);
}

template<class Integer> auto
is_boundary(const md_array_view<Integer,2>& pe, Integer i) -> bool {
  return (pe(i,0)==0) || (pe(i,1)==0); // 0 means no parent element, as per CGNS SIDS 7.3
}
template bool is_boundary<I4>(const md_array_view<I4,2>& pe, I4 i);
template bool is_boundary<I8>(const md_array_view<I8,2>& pe, I8 i);

auto
elts_ranges_are_contiguous(const tree_range& elt_pools) -> bool {
  STD_E_ASSERT(std::is_sorted(begin(elt_pools),end(elt_pools),compare_by_range));
  int nb_of_elt_types = elt_pools.size();
  for (int i=0; i<nb_of_elt_types-1; ++i) {
    auto current_range = element_range(elt_pools[i]);
    auto next_range = element_range(elt_pools[i+1]);
    return contiguous(current_range,next_range);
  }
  return true;
}
auto
elts_types_are_unique(const tree_range& elt_pools) -> bool {
  STD_E_ASSERT(std::is_sorted(begin(elt_pools),end(elt_pools),compare_by_elt_type));
  auto last = std::adjacent_find(begin(elt_pools),end(elt_pools),equal_by_elt_type);
  return last==end(elt_pools);
}


} // cgns
