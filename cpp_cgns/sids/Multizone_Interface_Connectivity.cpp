#include "cpp_cgns/sids/Multizone_Interface_Connectivity.hpp"

#include "std_e/future/contract.hpp"
#include "cpp_cgns/tree_manip.hpp"

// SEE https://cgns.github.io/CGNS_docs_current/sids/cnct.html
// From SIDS §8: Multizone Interface Connectivity
namespace cgns {

std::string ZoneDonorName(const tree& gc) {
  STD_E_ASSERT(gc.label=="GridConnectivity_t");
  return to_string(gc.value);
}

} // cgns
