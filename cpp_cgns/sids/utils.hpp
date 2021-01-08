#include "cpp_cgns/node_manip.hpp"

#include "cpp_cgns/sids/cgnslib.h"
#include "std_e/utils/integer_range.hpp"


namespace cgns {


auto
correct_bc_data_arrays_according_to_SIDS(tree& bcdata_node) -> void;

auto
nb_of_elements(tree& elements_node) -> I8;


auto 
element_type(const tree& elements_node) -> ElementType_t;
auto
element_range(const tree& element_node) -> std_e::integer_closed_range<I8>;


template<class I, class Tree> auto
regular_elements_connectivities(tree& elements_node) -> md_array_view<I,2>;


auto
compare_by_range(const tree& elts_node0, const tree& elts_node1) -> bool;
auto
equal_by_elt_type(const tree& elts_node0, const tree& elts_node1) -> bool;
auto
compare_by_elt_type(const tree& elts_node0, const tree& elts_node1) -> bool;


template<class I> auto
is_boundary(const md_array_view<I,2>& parent_elts, I i) -> bool;

template<class Array> auto
// requires Array, size==2, value_type==Integer
face_is_boundary(const Array& parent_elts) -> bool {
  return (parent_elts[0]==0) || (parent_elts[1]==0); // 0 means no parent element, as per CGNS SIDS 7.3
}


auto
elts_ranges_are_contiguous(const tree_range& elt_pools) -> bool;
auto
elts_types_are_unique(const tree_range& elt_pools) -> bool;


} // cgns
