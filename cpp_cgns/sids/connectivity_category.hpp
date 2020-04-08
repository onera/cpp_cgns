#pragma once


#include "cpp_cgns/sids/elements_utils.hpp"


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
connectivity_category_of(const tree& e) -> connectivity_category;

constexpr auto
is_interleaved(connectivity_category cat) -> bool {
  if (cat==interleaved_ngon || cat==interleaved_nface || cat==interleaved_mixed) return true;
  return false;
}

template<connectivity_category cat> inline auto size_of_type(int n);
template<> inline auto size_of_type<            ngon >(int n) { return n; };
template<> inline auto size_of_type<interleaved_ngon >(int n) { return n; };
template<> inline auto size_of_type<            nface>(int n) { return n; };
template<> inline auto size_of_type<interleaved_nface>(int n) { return n; };
template<> inline auto size_of_type<            mixed>(int n) { return number_of_nodes(ElementType_t(n)); }; // TODO perfo number_of_nodes (inline+correct!!)
template<> inline auto size_of_type<interleaved_mixed>(int n) { return number_of_nodes(ElementType_t(n)); };


} // cgns
