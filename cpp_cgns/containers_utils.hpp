#pragma once


#include "cpp_cgns/array.hpp"


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
md_array<T,rank> make_md_array(cgns_allocator& alloc) {
  return md_array<T,rank>(make_cgns_vector<T>(alloc));
}

template<class T, class Multi_index, int rank = Multi_index::rank>
md_array<T,rank> make_md_array(Multi_index dims, cgns_allocator& alloc) {
  std_e::dyn_shape<I8,rank> shape = {std::move(dims)};
  return md_array<T,rank>(make_cgns_vector<T>(alloc),std::move(shape));
}

template<class T, int N=1>
md_array<T,1> make_md_array(std::initializer_list<T> l, cgns_allocator& alloc) {
  static_assert(N==1,"N must be 1. It is an optional and unnecessary template argument, here for consistency");
  auto sz = l.size();
  return md_array<T,1>(l,make_cgns_vector<T>(sz,alloc));
}
template<class T, int N=2>
md_array<T,2> make_md_array(std::initializer_list<std::initializer_list<T>> ll, cgns_allocator& alloc) {
  static_assert(N==2,"N must be 2. It is an optional and unnecessary template argument, here for consistency");
  auto sz = ll.size() * std::begin(ll)->size();
  return md_array<T,2>(ll,make_cgns_vector<T>(sz,alloc));
}
// md_array }


} // cgns
