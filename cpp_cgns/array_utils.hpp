#pragma once


#include "cpp_cgns/array_types.hpp"


namespace cgns {


// cgns_vector {
template<class T>
cgns_vector<T> make_cgns_vector(cgns_allocator& alloc) {
  return cgns_vector<T>(cgns_std_allocator<T>(&alloc));
}

template<class T>
cgns_vector<T> make_cgns_vector(size_t n, cgns_allocator& alloc) {
  return cgns_vector<T>(n,cgns_std_allocator<T>(&alloc));
}

template<class T>
cgns_vector<T> make_cgns_vector(std::initializer_list<T> l, cgns_allocator& alloc) {
  return cgns_vector<T>(l,cgns_std_allocator<T>(&alloc));
}
// cgns_vector }


// md_array {
template<class T, int rank>
auto allocate_md_array(std_e::multi_index<I8,rank> dims, cgns_allocator& alloc) {
  std_e::dyn_shape<I8,rank> shape(std::move(dims));
  I8 sz = shape.size();
  T* ptr = allocate<T>(alloc,sz);
  std_e::span<T> mem_view {ptr,sz};
  return md_array_view<T,rank>(mem_view,shape);
}

template<class T>
md_array_view<T,2> make_md_array(std::initializer_list<std::initializer_list<T>> ll, cgns_allocator& alloc) {
  I8 sz = ll.size() * std::begin(ll)->size();
  T* ptr = allocate<T>(alloc,sz);
  std_e::span<T> mem_view {ptr,sz};
  return md_array_view<T,2>(ll,mem_view);
}
// md_array }


} // cgns
