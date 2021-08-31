#pragma once


#include "cpp_cgns/cgns.hpp"
#include "std_e/buffer/buffer.hpp"
#include "std_e/multi_array/multi_array.hpp"


namespace cgns {


constexpr int dyn_rank = std_e::dynamic_size;

template<class T, int rank>
using md_array_view = std_e::dyn_multi_array_view<T,rank,I8>;

template<class T, int rank, class A = std_e::buffer_mallocator>
using md_array = std_e::multi_array< std_e::buffer_vector<T,A> , std_e::dyn_shape<I8,rank>>;


// md_array creation {
template<class T, int rank, class A = std_e::buffer_mallocator> auto
make_md_array(std_e::multi_index<I8,rank> dims, A alloc = {}) {
  std_e::dyn_shape<I8,rank> shape(std::move(dims));
  I8 sz = shape.size();
  auto mem = std_e::make_buffer_vector<T>(sz,std::move(alloc));
  return md_array<T,rank,A>(mem,shape);
}

template<class T, class A, int rank> auto
make_md_array(std_e::buffer_vector<T,A>&& v, std_e::multi_index<I8,rank> dims) {
  std_e::dyn_shape<I8,rank> shape(std::move(dims));
  return md_array<T,rank,A>(std::move(v),shape);
}

template<class T, class A = std_e::buffer_mallocator> auto
make_md_array(std::initializer_list<std::initializer_list<T>> ll, A alloc = {}) {
  int n_row = ll.size();
  int n_col = ll.begin()->size();
  std_e::multi_index<I8,2> dims = {n_row,n_col};
  md_array<T,2,A> res = make_md_array<T>(dims,alloc);
  int i=0;
  for (const auto& l : ll) {
    int j=0;
    for (const auto& x : l) {
      res(i,j) = x;
      ++j;
    }
    ++i;
  }
  return res;
}
// md_array creation }


} // cgns
