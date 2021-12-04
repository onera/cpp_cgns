#include "cpp_cgns/sids/connectivity_category.hpp"

#include "cpp_cgns/sids/utils.hpp"

namespace cgns {

template<class I> auto 
connectivity_category_of(const tree& e) -> connectivity_category {
  STD_E_ASSERT(label(e)=="Elements_t");
  auto elt_type = element_type(e);
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

// explicit instanciations (do not pollute the header for only 2 instanciations)
template auto connectivity_category_of<I4>(const tree& e) -> connectivity_category;
template auto connectivity_category_of<I8>(const tree& e) -> connectivity_category;

} // cgns
