#if __cplusplus > 201703L
#include "cpp_cgns/sids/Grid_Coordinates_Elements_and_Flow_Solution.hpp"

namespace cgns {

template<class I> auto
is_boundary_partitioned_element_section(const tree& e) -> bool {
  STD_E_ASSERT(label(e)=="Elements_t");
  return ElementSizeBoundary(e) != 0; // per CGNS SIDS 7.3
}

// explicit instanciations (do not pollute the header for only 2 instanciations)
template auto is_boundary_partitioned_element_section<I4>(const tree& e) -> bool;
template auto is_boundary_partitioned_element_section<I8>(const tree& e) -> bool;

} // cgns
#endif // C++>17
