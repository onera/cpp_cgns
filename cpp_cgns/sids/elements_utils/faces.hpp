#pragma once


#include "cpp_cgns/sids/elements_utils.hpp"


namespace cgns {


constexpr auto
is_regular_face_element(const element_traits& x) {
  return x.dimension==2 && x.elt_type!=NGON_n;
}
constexpr auto
_compute_n_face_types() -> int {
  return std_e::count_if(begin(elements_traits),end(elements_traits) , is_regular_face_element);
}
constexpr auto
_compute_all_face_types() {
  constexpr int N = _compute_n_face_types();
  std::array<ElementType_t,N> fs;
  auto first = begin(fs);
  for (const element_traits& x : elements_traits) {
    if (is_regular_face_element(x)) {
      *first++ = x.elt_type;
    }
  }
  return fs;
}


//inline constexpr int n_face_type = _compute_n_face_types();
//inline constexpr std::array all_face_types = _compute_all_face_types();
inline constexpr int n_face_type = 2;
inline constexpr std::array all_face_types = std_e::make_array(TRI_3,QUAD_4);


template<class T>
using array_by_face_types = std::array<T,n_face_type>;


template<class T> auto
_get_face_type(T& x, ElementType_t face_type) -> auto& {
  STD_E_ASSERT(is_regular_face_element(elements_traits[face_type]));
  auto it = std::ranges::find(all_face_types,face_type);
  auto idx = it-begin(cgns::all_face_types);
  return x[idx];
}
template<class T> auto
at_face_type(      array_by_face_types<T>& x, ElementType_t face_type) ->       T& {
  return _get_face_type(x,face_type);
}
template<class T> auto
at_face_type(const array_by_face_types<T>& x, ElementType_t face_type) -> const T& {
  return _get_face_type(x,face_type);
}


} // cgns
