#include "cpp_cgns/exception.hpp"
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
    bcdata_array_node.value.dims = {1,bcdata_array_node.value.dims[0]};
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
element_range__impl(I, const tree& elements_node) -> std_e::integer_closed_range<I8> {
  auto elt_range = ElementRange<I>(elements_node);
  return {elt_range[0],elt_range[1]};
}
auto
element_range(const tree& elements_node) -> std_e::integer_closed_range<I8> {
  STD_E_ASSERT(elements_node.label=="Elements_t");
  return dispatch_I4_I8(
    LIFT(element_range__impl),
    elements_node
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
