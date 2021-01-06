#pragma once


#include "cpp_cgns/sids/elements_utils.hpp"
#include "cpp_cgns/exception.hpp"


namespace cgns {


enum connectivity_category {
  homogenous,
  ngon,
  nface,
  mixed,
  interleaved_ngon,
  interleaved_nface,
  interleaved_mixed,
  nb_connectivity_categories
};
using all_connectivity_categories = std::make_integer_sequence<int,nb_connectivity_categories>;

template<class I> auto 
connectivity_category_of(const tree& elements_t) -> connectivity_category;

constexpr auto
is_interleaved(connectivity_category cat) -> bool {
  if (cat==interleaved_ngon || cat==interleaved_nface || cat==interleaved_mixed) return true;
  return false;
}

template<connectivity_category cat, class I> constexpr auto
size_of_type(I n) {
  if constexpr (cat==ngon || cat==interleaved_ngon || cat==nface || cat==interleaved_nface) {
    return n;
  } else if constexpr (cat==mixed || cat==interleaved_mixed) {
    return number_of_nodes(n);
  } else {
    throw cgns_exception("homogenous or unknown connectivity category");
  }
}

} // cgns
