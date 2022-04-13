#pragma once

#include "cpp_cgns/tree.hpp"
#include "cpp_cgns/tree_manip.hpp"
#include "std_e/future/span.hpp"
#include "std_e/future/span.hpp"
#include "std_e/utils/enum.hpp"

// SEE https://cgns.github.io/CGNS_docs_current/sids/cnct.html
// From SIDS §8: Building-Block Structure Definitions
namespace cgns {

STD_E_ENUM(GridLocation_t,
  GridLocationNull,
  GridLocationUserDefined,
  Vertex,
  CellCenter,
  FaceCenter,
  IFaceCenter,
  JFaceCenter,
  KFaceCenter,
  EdgeCenter
);

std::string GridLocation(const tree& t);

template<class I, class Tree> auto
Rind(Tree& t) {
  return get_child_value_by_label<I>(t,"Rind_t");
}

} // cgns
