#include "cpp_cgns/sids/connectivity_category.hpp"


namespace cgns {


auto 
connectivity_category_of(const tree& e) -> connectivity_category {
  STD_E_ASSERT(e.label=="Elements_t");
  auto elt_type = ElementType(e);
  bool has_offsets = has_child_of_name(e,"ElementStartOffset");
  if (elt_type==NGON_n) {
    if (has_offsets) { return connectivity_category::ngon;             }
    else             { return connectivity_category::interleaved_ngon; }
  }
  if (elt_type==NFACE_n) {
    if (has_offsets) { return connectivity_category::nface;             }
    else             { return connectivity_category::interleaved_nface; }
  }
  if (elt_type==MIXED) {
    if (has_offsets) { return connectivity_category::mixed;             }
    else             { return connectivity_category::interleaved_mixed; }
  }
  return connectivity_category::homogenous;
}


} // cgns
