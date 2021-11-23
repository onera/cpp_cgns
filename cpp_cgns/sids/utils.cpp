#include "cpp_cgns/sids/utils.hpp"

#include "cpp_cgns/tree_manip.hpp"
#include "cpp_cgns/sids/elements_utils.hpp"
#include "cpp_cgns/dispatch.hpp"
#include "std_e/multi_array/utils.hpp"
#include "std_e/base/lift.hpp"


namespace cgns {


auto
correct_bc_data_arrays_according_to_SIDS(tree& bcdata_node) -> void {
  // SIDS: boundary field should have shape {1,PointList size}
  tree_range bcdata_array_nodes = get_children_by_label(bcdata_node,"DataArray_t");
  for (tree& bcdata_array_node : bcdata_array_nodes) {
    cons_reshape(bcdata_array_node.value,{1,bcdata_array_node.value.extent(0)});
  }
}

template<class I> auto
element_type__impl(I, const tree& elements_node) -> ElementType_t {
  return (ElementType_t) ElementType<I>(elements_node);
}
auto
element_type(const tree& elements_node) -> ElementType_t {
  STD_E_ASSERT(elements_node.label=="Elements_t");
  return dispatch_I4_I8(
    LIFT(element_type__impl),
    elements_node
  );
}

template<class I> auto
range_to_interval(I, const tree& range_node) -> cgns::interval<I8> {
  auto range = view_as_span<I>(range_node.value);
  return {range[0],range[1]};
}
auto
element_range(const tree& elements_node) -> cgns::interval<I8> {
  STD_E_ASSERT(elements_node.label=="Elements_t");
  return dispatch_I4_I8(
    LIFT(range_to_interval),
    get_child_by_name(elements_node,"ElementRange")
  );
}

auto
point_range_to_interval(const tree& pr) -> cgns::interval<I8> {
  STD_E_ASSERT(pr.label=="IndexRange_t");
  return dispatch_I4_I8(
    LIFT(range_to_interval),
    pr
  );
}


auto
nb_of_elements(const tree& elements_node) -> I8 {
  STD_E_ASSERT(elements_node.label=="Elements_t");
  return length(element_range(elements_node));
}


auto
compare_by_range(const tree& elts_node0, const tree& elts_node1) -> bool {
  STD_E_ASSERT(elts_node0.label=="Elements_t");
  STD_E_ASSERT(elts_node1.label=="Elements_t");
  return element_range(elts_node0).first() < element_range(elts_node1).first();
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


} // cgns
