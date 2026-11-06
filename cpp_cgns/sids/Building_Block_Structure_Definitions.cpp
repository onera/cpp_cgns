#if __cplusplus > 201703L
#include "cpp_cgns/sids/Building_Block_Structure_Definitions.hpp"

#include "cpp_cgns/tree_manip.hpp"

namespace cgns {

std::string GridLocation(const tree& t) {
  const tree& gl = get_child_by_name(t,"GridLocation");
  return to_string(value(gl));
}

} // cgns
#endif // C++>17
