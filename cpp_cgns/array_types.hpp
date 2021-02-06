#pragma once


#include "cpp_cgns/cgns.hpp"
#include "cpp_cgns/allocator.hpp"
#include "std_e/multi_array/multi_array.hpp"


namespace cgns {


constexpr int dyn_rank = std_e::dynamic_size;

template<class T, int rank> using md_array_view = std_e::dyn_multi_array_view<T,rank,I8>;

template<class T, int rank> using md_array = std_e::multi_array< cgns_vector<T> , std_e::dyn_shape<I8,rank>>;


} // cgns
