#pragma once


#include "cpp_cgns/cgns_allocator.hpp"
#include "std_e/multi_array/utils.hpp"


namespace cpp_cgns {


template<class T> using cgns_vector = std::vector<T, cgns_std_allocator<T>>;


template<class T>
cgns_vector<T> make_cgns_vector(cgns_allocator& alloc) {
  return cgns_vector<T>(cgns_std_allocator<T>(&alloc));
}

template<class T>
cgns_vector<T> make_cgns_vector(size_t n, cgns_allocator& alloc) {
  return cgns_vector<T>(n,cgns_std_allocator<T>(&alloc));
}

template<class T>
cgns_vector<T> make_cgns_vector(const std::initializer_list<T>& l, cgns_allocator& alloc) {
  return cgns_vector<T>(l,cgns_std_allocator<T>(&alloc));
}


} // cpp_cgns
