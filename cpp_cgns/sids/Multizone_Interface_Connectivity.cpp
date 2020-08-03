#include "cpp_cgns/sids/Multizone_Interface_Connectivity.hpp"

#include "std_e/future/contract.hpp"
#include "cpp_cgns/node_manip.hpp"
#include "cpp_cgns/tree_manip.hpp"
#include "std_e/future/contract.hpp"

// SEE https://cgns.github.io/CGNS_docs_current/sids/cnct.html
// From SIDS §8: Multizone Interface Connectivity
namespace cgns {

std::string ZoneDonorName(const tree& gc) {
  STD_E_ASSERT(gc.label=="GridConnectivity_t");
  return to_string(gc.value);
}

std::string GridLocation(const tree& t) {
  const tree& gl = get_child_by_name(t,"GridLocation");
  return to_string(gl.value);
}

} // cgns
