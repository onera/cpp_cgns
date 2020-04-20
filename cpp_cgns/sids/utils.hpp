#include "cpp_cgns/node_manip.hpp"

#include "cpp_cgns/cgnslib.h"
#include "std_e/utils/integer_range.hpp"


namespace cgns {


void correct_bc_data_arrays_according_to_SIDS(tree& bcdata_node);

ElementType_t element_type(tree& elements_node);
std_e::integer_closed_range_ref<I4> element_range(tree& element_node);
md_array_view<I4,2> regular_elements_connectivities(tree& elements_node);


bool compare_by_range(tree& elts_node0, tree& elts_node1);
bool equal_by_elt_type(tree& elts_node0, tree& elts_node1);
bool compare_by_elt_type(tree& elts_node0, tree& elts_node1);

template<class I>
bool is_boundary(const md_array_view<I,2>& parent_elts, I i);

template<class Array2>
// requires Array2 is Array, size==2, value_type==Integer
bool face_is_boundary(const Array2& parent_elts) {
  return (parent_elts[0]==0) || (parent_elts[1]==0); // 0 means no parent element, as per CGNS SIDS 7.3
}


} // cgns
