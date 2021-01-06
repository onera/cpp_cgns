#include "cpp_cgns/sids/Grid_Coordinates_Elements_and_Flow_Solution.hpp"

namespace cgns {

template<class I> auto
is_boundary_partitionned_element_pool(const tree& e) -> bool {
  STD_E_ASSERT(e.label=="Elements_t");
  return ElementSizeBoundary<I>(e) != 0; // per CGNS SIDS 7.3
}

// explicit instanciations (do not pollute the header for only 2 instanciations)
template auto is_boundary_partitionned_element_pool<I4>(const tree& e) -> bool;
template auto is_boundary_partitionned_element_pool<I8>(const tree& e) -> bool;

} // cgns
