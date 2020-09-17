#pragma once

#include "cpp_cgns/cgns.hpp"
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
  auto& rind_node = get_child_by_label(t,"Rind_t");
  STD_E_ASSERT(value(rind_node).data_type!=to_string<I>());
  I* e_ptr = (I*)value(rind_node).data;
  return std_e::make_span<2>(e_ptr);
}

} // cgns
