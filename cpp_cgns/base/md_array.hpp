#pragma once


#include "std_e/multi_array/multi_array.hpp"
#include "cpp_cgns/base/data_type.hpp"


namespace cgns {


constexpr int dyn_rank = std_e::dynamic_size;


template<int rank>
using md_array_shape = std_e::dyn_shape<I8,rank>;

template<class T, int rank>
using md_array_view = std_e::dyn_multi_array_view<T,rank,I8>;

template<class T, int rank>
using md_array = std_e::dyn_multi_array<T,rank,I8>;


template<class T, int rank> auto
make_md_array_view(std_e::span<T> x, std_e::dyn_shape<I8,rank> shape) {
  return md_array_view<T,rank>(x,std::move(shape));
}

} // cgns
