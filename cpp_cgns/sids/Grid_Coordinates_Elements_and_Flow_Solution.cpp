#include "cpp_cgns/sids/Grid_Coordinates_Elements_and_Flow_Solution.hpp"

namespace cgns {

bool is_boundary_partitionned_element_pool(const tree& e) {
  STD_E_ASSERT(e.label=="Elements_t");
  return ElementSizeBoundary<I4>(e) != 0; // per CGNS SIDS 7.3 // TODO I4 -> I4/I8
}

} // cgns
