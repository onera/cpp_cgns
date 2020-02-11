#include "cpp_cgns/cpp_cgns_utils.hpp"

#include "cpp_cgns/cgnslib.hpp"
#include "std_e/utils/integer_range.hpp"


namespace cpp_cgns {


std::array<I4,3> dims_of_unstruct_zone(tree& zone);
I4 number_of_nodes_of_unstruct_zone(tree& zone);

void correct_bc_data_arrays_according_to_SIDS(tree& bcdata_node);

CGNS_ENUMT(ElementType_t) element_type(tree& elements_node);
std_e::integer_closed_range_ref<I4> element_range(tree& element_node);
md_array_view<I4,2> regular_elements_connectivities(tree& elements_node);


bool compare_by_range(tree& elts_node0, tree& elts_node1);
bool equal_by_elt_type(tree& elts_node0, tree& elts_node1);
bool compare_by_elt_type(tree& elts_node0, tree& elts_node1);


} // cpp_cgns
