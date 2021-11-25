#pragma once


#include "cpp_cgns/base/node_value.hpp"
#include "cpp_cgns/base/md_array.hpp"
#include "std_e/multi_index/cartesian_product_size.hpp"
#include "std_e/multi_index/multi_index_range.hpp"
#include "std_e/future/span.hpp"
#include "std_e/multi_index/utils.hpp"


namespace cgns {


/// node_value -> span {
template<class T, int rank = dyn_rank> auto
view_as_span(node_value& x) -> std_e::span<T,rank> {
  STD_E_ASSERT(x.data_type()==to_string<T>());
  STD_E_ASSERT(std_e::is_one_dimensional(x.extent()));
  return std_e::span<T,rank>(data_as<T>(x), std_e::cartesian_product_size(x.extent()));
}
template<class T, int rank = dyn_rank> auto
view_as_span(const node_value& x) -> std_e::span<const T,rank> {
  STD_E_ASSERT(x.data_type()==to_string<T>());
  STD_E_ASSERT(std_e::is_one_dimensional(x.extent()));
  return std_e::span<const T,rank>(data_as<T>(x), std_e::cartesian_product_size(x.extent()));
}
/// node_value -> span }


/// node_value -> md_array_view<T> {
template<class T, int rank> auto
view_as_md_array(node_value& x) -> md_array_view<T,rank> {
  STD_E_ASSERT(x.data_type()==to_string<T>());
  STD_E_ASSERT(rank==dyn_rank || int(x.rank())==rank);
  std_e::dyn_shape<I8,rank> shape{x.extent()};
  std_e::span<T> mem_view {data_as<T>(x),shape.size()};
  return md_array_view<T,rank>(mem_view,shape);
}
template<class T, int rank> auto
view_as_md_array(const node_value& x) -> md_array_view<const T,rank> {
  STD_E_ASSERT(x.data_type()==to_string<T>());
  STD_E_ASSERT(rank==dyn_rank || int(x.rank())==rank);
  std_e::dyn_shape<I8,rank> shape{x.extent()};
  std_e::span<const T> mem_view {data_as<T>(x),shape.size()};
  return md_array_view<const T,rank>(mem_view,shape);
}
template<class T, int rank> auto
view_as_md_array(const node_value& x, std::vector<I8> dims) -> md_array_view<const T,rank> {
  STD_E_ASSERT(x.data_type()==to_string<T>());
  STD_E_ASSERT(rank==dyn_rank || int(x.rank())==rank);
  STD_E_ASSERT(x.size()==std_e::cartesian_product_size(dims));
  std_e::dyn_shape<I8,rank> shape{std::move(dims)};
  std_e::span<const T> mem_view {data_as<T>(x),shape.size()};
  return md_array_view<const T,rank>(mem_view,shape);
}
/// node_value -> md_array_view<T> }


/// node_value -> any array {
template<class T, int N = 1, class Node_value> auto
view_as_array(Node_value& value) {
  if constexpr (N==1) {
    return view_as_span<T>(value);
  } else {
    return view_as_md_array<T,N>(value);
  }
}
/// node_value -> any array }


} // cgns }
