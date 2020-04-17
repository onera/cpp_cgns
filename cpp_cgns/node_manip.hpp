#pragma once


#include "cpp_cgns/cgns.hpp"
#include "std_e/multi_array/multi_array.hpp"
#include "cpp_cgns/allocator.hpp"
#include "std_e/multi_index/cartesian_product.hpp"
#include "std_e/multi_index/multi_index_range.hpp"
#include "std_e/future/span.hpp"
#include "std_e/multi_index/utils.hpp"
#include "std_e/base/not_implemented_exception.hpp"
#include "cpp_cgns/array.hpp"
#include "cpp_cgns/containers_utils.hpp"


namespace cgns {


// span/vector/md_array_view<T> <-> node_value {

/// span/vector/md_array_view<T> -> node_value {
template<class T, index_t N>
node_value view_as_node_value(std_e::span<T,N> x) {
  return {to_string<T>(),{(I8)x.size()},x.data()};
}
template<class T>
node_value view_as_node_value(cgns_vector<T>& x) {
  return view_as_node_value(std_e::make_span(x));
}
template<class T0, class T1>
node_value view_as_node_value(std_e::multi_array<T0,T1>& x) {
  using T = typename std_e::multi_array<T0,T1>::value_type;
  return {to_string<T>(),std_e::convert_to<std::vector<I8>>(x.extent()),x.data()};
}
/// span/vector/md_array_view<T> -> node_value }

/// node_value -> md_array_view<T> {
template<class T, int rank>
md_array_view<T,rank> view_as_md_array(node_value& x) {
  STD_E_ASSERT(x.data_type==to_string<T>());
  STD_E_ASSERT(x.dims.size()==rank);
  std_e::memory_view<T*> mem_view {(T*)x.data};
  std_e::dyn_shape<I8,rank> shape{x.dims};
  return md_array_view<T,rank>(mem_view,shape);
}
template<class T, int rank>
md_array_view<const T,rank> view_as_md_array(const node_value& x) {
  STD_E_ASSERT(x.data_type==to_string<T>());
  STD_E_ASSERT(x.dims.size()==rank);
  std_e::memory_view<const T*> mem_view {(const T*)x.data};
  std_e::dyn_shape<I8,rank> shape{x.dims};
  return md_array_view<const T,rank>(mem_view,shape);
}
/// node_value -> md_array_view<T> }


/// T*,extent -> md_array_view<T> {
template<class T, class Multi_index, int rank = std_e::rank_of<Multi_index>>
md_array_view<T,rank> view_as_md_array(T* ptr, Multi_index&& dims) {
  std_e::memory_view<T*> mem_view {ptr};
  std_e::dyn_shape<I8,rank> shape{FWD(dims)};
  return md_array_view<T,rank>(mem_view,shape);
}
/// T*,extent -> md_array_view<T> }


/// node_value -> span {
template<class T, int rank = dyn_rank>
std_e::span<T,rank> view_as_span(node_value& x) {
  STD_E_ASSERT(x.data_type==to_string<T>());
  STD_E_ASSERT(std_e::is_one_dimensional(x.dims));
  return std_e::span<T,rank>((T*)x.data,std_e::cartesian_product(x.dims));
}
template<class T, int rank = dyn_rank>
std_e::span<const T,rank> view_as_span(const node_value& x) {
  STD_E_ASSERT(x.data_type==to_string<T>());
  STD_E_ASSERT(std_e::is_one_dimensional(x.dims));
  return std_e::span<const T,rank>((const T*)x.data,std_e::cartesian_product(x.dims));
}
/// node_value -> span }

// span/vector/md_array_view<T> <-> node_value }

// to_string {
inline constexpr int default_threshold_to_print_whole_array = 10;
std::string to_string(const node_value& x, int threshold = default_threshold_to_print_whole_array);
std::string to_string(const tree& t, int threshold = default_threshold_to_print_whole_array);
// to_string }

// to_string }
inline
tree deep_copy(const tree& t, cgns_allocator& alloc) {
  throw std_e::not_implemented_exception();
}

// node_value creation from "small" arrays {
template<class Range>
node_value create_node_value_1d__impl(const Range& r, cgns_allocator& alloc) {
  using T = typename Range::value_type;
  I8 n = r.size();
  T* data = allocate<T>(alloc,n);
  std::copy_n(begin(r),n,data);
  return {to_string<T>(),{n},data};
}

template<class Range>
node_value create_node_value_1d(const Range& r, cgns_allocator& alloc) {
  return create_node_value_1d__impl(r,alloc);
}
template<class T>
node_value create_node_value_1d(std::initializer_list<T> l, cgns_allocator& alloc) {
  return create_node_value_1d__impl(l,alloc);
}
inline
node_value create_string_node_value(const std::string& s, cgns_allocator& alloc) {
  return create_node_value_1d__impl(s,alloc);
}
// node_value creation from "small" arrays }


// md_array allocation by cgns allocator {
template<class T, int rank>
auto allocate_md_array(const std_e::multi_index<I8,rank>& dims, cgns_allocator& alloc) {
  T* ptr = allocate<T>(alloc,std_e::cartesian_product(dims));
  std_e::memory_view<T*> mem_view {ptr};
  std_e::dyn_shape<I8,rank> shape(dims);
  return md_array_view<T,rank>(mem_view,shape);
}
// md_array allocation by cgns allocator }


} // cgns }
