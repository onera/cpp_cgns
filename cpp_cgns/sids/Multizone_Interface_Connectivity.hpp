#pragma once

#include "cpp_cgns/cgns.hpp"

// SEE https://cgns.github.io/CGNS_docs_current/sids/cnct.html
// From SIDS §8: Multizone Interface Connectivity
namespace cgns {

std::string ZoneDonorName(const tree& gc);
std::string GridLocation(const tree& t);

} // cgns
