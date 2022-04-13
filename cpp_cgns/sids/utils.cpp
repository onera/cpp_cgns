#if __cplusplus > 201703L
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
    cons_reshape(value(bcdata_array_node),{1,value(bcdata_array_node).extent(0)});
  }
}

auto
element_type(const tree& e) -> ElementType_t {
  I8 e_type = ElementType(e); // convert to I8, because it works with I4 and I8
  return ElementType_t(e_type);
}

template<class I> auto
range_to_interval(I, const tree& range_node) -> cgns::interval<I8> {
  auto range = get_value<I>(range_node);
  return {range[0],range[1]};
}
auto
element_range(const tree& elements_node) -> cgns::interval<I8> {
  STD_E_ASSERT(label(elements_node)=="Elements_t");
  return dispatch_I4_I8(
    LIFT(range_to_interval),
    get_child_by_name(elements_node,"ElementRange")
  );
}

auto
point_range_to_interval(const tree& pr) -> cgns::interval<I8> {
  STD_E_ASSERT(label(pr)=="IndexRange_t");
  return dispatch_I4_I8(
    LIFT(range_to_interval),
    pr
  );
}


auto
nb_of_elements(const tree& elements_node) -> I8 {
  STD_E_ASSERT(label(elements_node)=="Elements_t");
  return length(element_range(elements_node));
}


auto
compare_by_range(const tree& elts_node0, const tree& elts_node1) -> bool {
  STD_E_ASSERT(label(elts_node0)=="Elements_t");
  STD_E_ASSERT(label(elts_node1)=="Elements_t");
  return element_range(elts_node0).first() < element_range(elts_node1).first();
}
auto
equal_by_elt_type(const tree& elts_node0, const tree& elts_node1) -> bool {
  STD_E_ASSERT(label(elts_node0)=="Elements_t");
  STD_E_ASSERT(label(elts_node1)=="Elements_t");
  return element_type(elts_node0) == element_type(elts_node1);
}
auto
compare_by_elt_type(const tree& elts_node0, const tree& elts_node1) -> bool {
  STD_E_ASSERT(label(elts_node0)=="Elements_t");
  STD_E_ASSERT(label(elts_node1)=="Elements_t");
  return element_type(elts_node0) < element_type(elts_node1);
}


} // cgns
#endif // C++>17
