#pragma once


#include "cpp_cgns/array_types.hpp"


namespace cgns {


// cgns_vector {
template<class T, class Allocator = malloc_allocator> auto
make_cgns_vector(Allocator alloc = {}) {
  return cgns_vector<T>(cgns_std_allocator<T,Allocator>(std::move(alloc)));
}

template<class T, class Allocator = malloc_allocator> auto
make_cgns_vector(size_t n, Allocator alloc = {}) {
  return cgns_vector<T>(n,cgns_std_allocator<T,Allocator>(std::move(alloc)));
}

template<class T, class Allocator = malloc_allocator> auto
make_cgns_vector(std::initializer_list<T> l, Allocator alloc = {}) {
  return cgns_vector<T>(l,cgns_std_allocator<T,Allocator>(std::move(alloc)));
}
// cgns_vector }


// md_array {
/// returns view since the memory is owned by the allocator
template<class T, int rank, class Allocator = malloc_allocator> auto
make_md_array(std_e::multi_index<I8,rank> dims, Allocator alloc = {}) {
  std_e::dyn_shape<I8,rank> shape(std::move(dims));
  I8 sz = shape.size();
  auto mem = make_cgns_vector(sz,std::move(alloc));
  return md_array<T,rank>(mem,shape);
}

//template<class T, class Allocator = malloc_allocator> auto
//make_md_array(std::initializer_list<std::initializer_list<T>> ll, Allocator alloc = {}) {
template<class T> auto
make_md_array(std::initializer_list<std::initializer_list<T>> ll) {
  return md_array<T,2>(ll); // TODO extend std_e::multi_array to take an allocator
}
// md_array }


} // cgns
