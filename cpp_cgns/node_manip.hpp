#pragma once


#include "cpp_cgns/cgns.hpp"
#include "std_e/multi_array/multi_array.hpp"
#include "std_e/buffer/buffer.hpp"
#include "std_e/multi_index/cartesian_product_size.hpp"
#include "std_e/multi_index/multi_index_range.hpp"
#include "std_e/future/span.hpp"
#include "std_e/multi_index/utils.hpp"
#include "std_e/base/not_implemented_exception.hpp"
#include "cpp_cgns/md_array.hpp"
#include <initializer_list>


namespace cgns {


// span/vector/md_array <-> node_value {
/// span/vector/md_array -> node_value {
template<class T, class A> auto
make_node_value(std_e::buffer_vector<T,A>&& x) -> node_value {
  return { to_string<T>(),
           {(I8)x.size()},
           std_e::to_buffer(std::move(x)) };
}
template<class T, class A> auto
make_node_value_1(std_e::buffer_vector<T,A>&& x) -> node_value {
  return { to_string<T>(),
           {1,(I8)x.size()},
           std_e::to_buffer(std::move(x)) };
}

template<class T, int rank, class A> auto
make_node_value(md_array<T,rank,A>&& x) -> node_value {
  return { to_string<T>(),
           std_e::convert_to<std::vector<I8>>(x.extent()),
           std_e::to_buffer(std::move(x.underlying_range())) };
}

template<class T, class A = std_e::buffer_mallocator> auto
create_node_value(std::initializer_list<T> l, A alloc = {}) -> node_value {
  return make_node_value(std_e::make_buffer_vector(l,alloc));
}
template<class T, class A = std_e::buffer_mallocator> auto
create_scalar_node_value(T x, A alloc = {}) -> node_value {
  return create_node_value({x},alloc);
}
template<class T, class A = std_e::buffer_mallocator> auto
create_node_value_1(std::initializer_list<T> l, A alloc = {}) -> node_value {
  return make_node_value_1(std_e::make_buffer_vector(l,alloc));
}
template<class T, class A = std_e::buffer_mallocator> auto
create_node_value(std::initializer_list<std::initializer_list<T>> ll, A alloc = {}) -> node_value {
  return make_node_value(make_md_array(ll,alloc));
}
template<class A = std_e::buffer_mallocator> auto
create_string_node_value(const std::string& s, A alloc = {}) -> node_value {
  return make_node_value(std_e::make_buffer_vector(begin(s),end(s),alloc));
}

inline auto
view(node_value& x) -> node_value {
  return {x.data_type,x.dims,std_e::buffer_span(data(x))};
}

template<class T, ptrdiff_t N> auto
view_as_node_value(std_e::span<T,N> x) -> node_value {
  static_assert(!std::is_const_v<T>,"no way to ensure constness in CGNS tree");
  return {to_string<T>(), {(I8)x.size()}, buffer_view(x.data())};
}
template<class T, int rank> auto
view_as_node_value(md_array_view<T,rank>& x) -> node_value {
  return {to_string<T>(), std_e::convert_to<std::vector<I8>>(x.extent()), std_e::buffer_span{x.data()}};
}
/// span/vector/md_array -> node_value }

/// node_value -> md_array_view<T> {
template<class T, int rank> auto
view_as_md_array(node_value& x) -> md_array_view<T,rank> {
  STD_E_ASSERT(x.data_type==to_string<T>());
  STD_E_ASSERT(rank==std_e::dynamic_size || int(x.dims.size())==rank);
  std_e::dyn_shape<I8,rank> shape{x.dims};
  std_e::span<T> mem_view {(T*)data(x),shape.size()};
  return md_array_view<T,rank>(mem_view,shape);
}
template<class T, int rank> auto
view_as_md_array(const node_value& x) -> md_array_view<const T,rank> {
  STD_E_ASSERT(x.data_type==to_string<T>());
  STD_E_ASSERT(rank==std_e::dynamic_size || int(x.dims.size())==rank);
  std_e::dyn_shape<I8,rank> shape{x.dims};
  std_e::span<const T> mem_view {(const T*)data(x),shape.size()};
  return md_array_view<const T,rank>(mem_view,shape);
}
/// node_value -> md_array_view<T> }


/// T*,extent -> md_array_view<T> {
template<class T, class Multi_index, int rank = std_e::rank_of<Multi_index>> auto
view_as_md_array(T* ptr, Multi_index&& dims) -> md_array_view<T,rank> {
  std_e::dyn_shape<I8,rank> shape{FWD(dims)};
  std_e::span<T> mem_view {ptr,shape.size()};
  return md_array_view<T,rank>(mem_view,shape);
}
/// T*,extent -> md_array_view<T> }


/// node_value -> span {
template<class T, int rank = dyn_rank> auto
view_as_span(node_value& x) -> std_e::span<T,rank> {
  STD_E_ASSERT(x.data_type==to_string<T>());
  STD_E_ASSERT(std_e::is_one_dimensional(x.dims));
  return std_e::span<T,rank>((T*)data(x), std_e::cartesian_product_size(x.dims));
}
template<class T, int rank = dyn_rank> auto
view_as_span(const node_value& x) -> std_e::span<const T,rank> {
  STD_E_ASSERT(x.data_type==to_string<T>());
  STD_E_ASSERT(std_e::is_one_dimensional(x.dims));
  return std_e::span<const T,rank>((const T*)data(x), std_e::cartesian_product_size(x.dims));
}
/// node_value -> span }


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
// span/vector/md_array <-> node_value }

// to_string {
inline constexpr int default_threshold_to_print_whole_array = 10;
auto to_string(const node_value& x, int threshold = default_threshold_to_print_whole_array) -> std::string;
auto to_string(const tree& t, int threshold = default_threshold_to_print_whole_array) -> std::string;
// to_string }


} // cgns }
