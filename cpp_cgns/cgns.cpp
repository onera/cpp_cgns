#include "cpp_cgns/cgns.hpp"
#include "cpp_cgns/dispatch.hpp"
#include <algorithm>
#include "std_e/graph/algorithm/zip.hpp"
#include "std_e/graph/algorithm/algo_adjacencies.hpp"
#include "std_e/multi_index/cartesian_product_size.hpp"


namespace cgns {


// node_value comparison {
auto same_node_data(const node_value& x, const node_value& y) -> bool {
  return
      x.data_type==y.data_type
   && x.dims==y.dims
   && data(x)==data(y);
}


template<class T> auto
equal_node_data__impl(T, const node_value& x, const node_value& y) {
  I8 sz = std_e::cartesian_product_size(x.dims);
  auto x_ptr = (T*)data(x);
  auto y_ptr = (T*)data(y);
  return std::equal(x_ptr,x_ptr+sz,y_ptr);
}

auto operator==(const node_value& x, const node_value& y) -> bool {
  bool same_shape = x.data_type==y.data_type
                 && x.dims==y.dims;
  if (!same_shape) return false;
  if (data(x)==data(y)) return true;

  // Note: a bit-wise comparison is not enought
  //       because it does not work for floating point types
  return dispatch_on_data_type(
    x.data_type,
    [&x,&y](auto type){ return equal_node_data__impl(type,x,y); }
  );
}
auto operator!=(const node_value& x, const node_value& y) -> bool {
  return !(x==y);
}
// node_value comparison }


// tree comparisons {
auto
same_tree_structure(const tree& x, const tree& y) -> bool {
  auto xy = std_e::zip_graphs(x,y);
  auto different_node = [&](const auto& xy){
    const tree& x = xy.first;
    const tree& y = xy.second;
    bool same_node =  x.name==y.name  &&  x.label==y.label;
    return !same_node  ||  x.children.size() != y.children.size();
  };
  auto first_different_node = std_e::preorder_depth_first_find_adjacencies(xy,different_node);
  return first_different_node == last_root(xy);
}

auto
operator==(const tree& x, const tree& y) -> bool {
  auto xy = std_e::zip_graphs(x,y);
  auto different_node = [&](const auto& xy){
    const tree& x = xy.first;
    const tree& y = xy.second;
    bool same_node =  x.name==y.name  &&  x.label==y.label  &&  x.value==y.value;
    return !same_node  ||  x.children.size() != y.children.size();
  };
  auto first_different_node = std_e::preorder_depth_first_find_adjacencies(xy,different_node);
  return first_different_node == last_root(xy);
}

auto
operator!=(const tree& x, const tree& y) -> bool {
  return !(x==y);
}
// tree comparisons }


} // cgns
