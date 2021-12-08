#pragma once


#include "cpp_cgns/base/node_value.hpp"
#include "cpp_cgns/base/md_array.hpp"
#include "std_e/multi_index/cartesian_product_size.hpp"
#include "std_e/multi_index/multi_index_range.hpp"
#include "std_e/future/span.hpp"
#include "std_e/multi_index/utils.hpp"


namespace cgns {


// [Sphinx Doc] node value conversions {
template<class T,               class Node_value> auto view_as_span(Node_value& x);
template<class T, int rank    , class Node_value> auto view_as_md_array(Node_value& x);
template<class T, int rank    , class Node_value> auto view_as_md_array(Node_value& x, std::vector<I8> dims);
template<class T, int rank = 1, class Node_value> auto view_as_array(Node_value& x);
// [Sphinx Doc] node value conversions }


/// node_value -> span {
template<class T, class Node_value> auto
view_as_span(Node_value& x) {
  STD_E_ASSERT(x.data_type()==to_string<T>());
  STD_E_ASSERT(std_e::is_one_dimensional(x.extent()));
  return std_e::make_span(data_as<T>(x), x.size());
}
/// node_value -> span }


/// node_value -> md_array_view<T> {
template<class T, int rank, class Node_value> auto
view_as_md_array(Node_value& x, std::vector<I8> dims) {
  STD_E_ASSERT(x.data_type()==to_string<T>());
  STD_E_ASSERT(rank==dyn_rank || int(x.rank())==rank);
  STD_E_ASSERT(x.size()==std_e::cartesian_product_size(dims));
  std_e::dyn_shape<I8,rank> shape{std::move(dims)};
  auto mem_view = std_e::make_span(data_as<T>(x),shape.size());
  return make_md_array_view(mem_view,std::move(shape));
}
template<class T, int rank, class Node_value> auto
view_as_md_array(Node_value& x) {
  return view_as_md_array<T,rank>(x,x.extent());
}
/// node_value -> md_array_view<T> }


/// node_value -> any array {
template<class T, int rank, class Node_value> auto
view_as_array(Node_value& x) {
  if constexpr (rank==1) {
    if (!std_e::is_one_dimensional(x.extent())) {
      throw cgns_exception(
        "Trying to view an array of extent "+std_e::to_string(x.extent())+" as a mono-dimensional array"
      );
    }
    return view_as_span<T>(x);
  } else {
    if (x.rank()!=rank) {
      throw cgns_exception(
        "Trying to view an array of rank "+std::to_string(x.rank())+" as an array of rank "+std::to_string(rank)
      );
    }
    return view_as_md_array<T,rank>(x);
  }
}
/// node_value -> any array }


} // cgns }
