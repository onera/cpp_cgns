#pragma once


#include "cpp_cgns/cgns.hpp"
#include "std_e/multi_array/multi_array.hpp"
#include "cpp_cgns/allocator.hpp"
#include "std_e/multi_index/cartesian_product_size.hpp"
#include "std_e/multi_index/multi_index_range.hpp"
#include "std_e/future/span.hpp"
#include "std_e/multi_index/utils.hpp"
#include "std_e/base/not_implemented_exception.hpp"
#include "cpp_cgns/array_types.hpp"
#include "cpp_cgns/array_utils.hpp"


namespace cgns {


// span/vector/md_array_view<T> <-> node_value {

/// span/vector/md_array_view<T> -> node_value {
template<class T, ptrdiff_t N> auto
view_as_node_value(std_e::span<T,N> x) -> node_value {
  static_assert(!std::is_const_v<T>,"no way to ensure constness in CGNS tree");
  return {to_string<T>(), {(I8)x.size()}, x.data()};
}
template<class T> auto
view_as_node_value(cgns_vector<T>& x) -> node_value {
  return view_as_node_value(std_e::make_span(x));
}
template<class T0, class T1> auto
view_as_node_value(std_e::multi_array<T0,T1>& x) -> node_value {
  using T = typename std_e::multi_array<T0,T1>::value_type;
  return {to_string<T>(), std_e::convert_to<std::vector<I8>>(x.extent()), x.data()};
}
template<class T, ptrdiff_t N> auto
view_as_node_value_1(std_e::span<T,N> x) -> node_value {
  static_assert(!std::is_const_v<T>,"no way to ensure constness in CGNS tree");
  return {to_string<T>(), {1,(I8)x.size()}, x.data()};
}
template<class T> auto
view_as_node_value_1(cgns_vector<T> x) -> node_value {
  return view_as_node_value_1(std_e::make_span(x));
}
/// span/vector/md_array_view<T> -> node_value }

/// node_value -> md_array_view<T> {
template<class T, int rank> auto
view_as_md_array(node_value& x) -> md_array_view<T,rank> {
  STD_E_ASSERT(x.data_type==to_string<T>());
  STD_E_ASSERT(rank==std_e::dynamic_size || int(x.dims.size())==rank);
  std_e::dyn_shape<I8,rank> shape{x.dims};
  std_e::span<T> mem_view {(T*)x.data,shape.size()};
  return md_array_view<T,rank>(mem_view,shape);
}
template<class T, int rank> auto
view_as_md_array(const node_value& x) -> md_array_view<const T,rank> {
  STD_E_ASSERT(x.data_type==to_string<T>());
  STD_E_ASSERT(rank==std_e::dynamic_size || int(x.dims.size())==rank);
  std_e::dyn_shape<I8,rank> shape{x.dims};
  std_e::span<const T> mem_view {(const T*)x.data,shape.size()};
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
  return std_e::span<T,rank>((T*)x.data, std_e::cartesian_product_size(x.dims));
}
template<class T, int rank = dyn_rank> auto
view_as_span(const node_value& x) -> std_e::span<const T,rank> {
  STD_E_ASSERT(x.data_type==to_string<T>());
  STD_E_ASSERT(std_e::is_one_dimensional(x.dims));
  return std_e::span<const T,rank>((const T*)x.data, std_e::cartesian_product_size(x.dims));
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

// span/vector/md_array_view<T> <-> node_value }

// to_string {
inline constexpr int default_threshold_to_print_whole_array = 10;
auto to_string(const node_value& x, int threshold = default_threshold_to_print_whole_array) -> std::string;
auto to_string(const tree& t, int threshold = default_threshold_to_print_whole_array) -> std::string;
// to_string }

// comparison {
auto same_node_data(const node_value& x, const node_value& y) -> bool;
auto equal_node_data(const node_value& x, const node_value& y) -> bool;
// comparison }

// node_value creation from "small" arrays {
template<class Range> auto
create_node_value_1d__impl(const Range& r, cgns_allocator& alloc) -> node_value {
  using T = typename Range::value_type;
  I8 n = r.size();
  T* data = allocate<T>(alloc,n);
  std::copy_n(begin(r),n,data);
  return {to_string<T>(), {n}, data};
}

template<class Range> auto
create_node_value_1d(const Range& r, cgns_allocator& alloc) -> node_value {
  return create_node_value_1d__impl(r,alloc);
}
template<class T> auto
create_node_value_1d(std::initializer_list<T> l, cgns_allocator& alloc) -> node_value {
  return create_node_value_1d__impl(l,alloc);
}
inline auto
create_string_node_value(const std::string& s, cgns_allocator& alloc) -> node_value {
  return create_node_value_1d__impl(s,alloc);
}
// node_value creation from "small" arrays }


} // cgns }
