#include "cpp_cgns/base/tree.hpp"


#include <algorithm>
#include "std_e/graph/algorithm/zip.hpp"
#include "std_e/graph/algorithm/algo_adjacencies.hpp"


namespace cgns {


// tree comparisons {
auto
same_tree_structure(const tree& x, const tree& y) -> bool {
  auto xy = std_e::zip_graphs(x,y);
  auto different_node = [&](const auto& xy){
    const tree& x = xy.first;
    const tree& y = xy.second;
    bool same_node_structure =  name(x)==name(y)  &&  label(x)==label(y);
    return !same_node_structure  ||  number_of_children(x) != number_of_children(y);
  };
  return !std_e::preorder_depth_first_find_adjacencies_b(xy,different_node);
}

auto
operator==(const tree& x, const tree& y) -> bool {
  auto xy = std_e::zip_graphs(x,y);
  auto different_node = [&](const auto& xy){
    const tree& x = xy.first;
    const tree& y = xy.second;
    bool same_node =  name(x)==name(y)  &&  label(x)==label(y)  &&  value(x)==value(y);
    return !same_node  ||  number_of_children(x) != number_of_children(y);
  };
  return !std_e::preorder_depth_first_find_adjacencies_b(xy,different_node);
}

auto
operator!=(const tree& x, const tree& y) -> bool {
  return !(x==y);
}
// tree comparisons }


// to_string {
auto to_string_impl(const tree& t, const std::string& indent, int threshold) -> std::string {
  // TODO replace by dfs
  static const std::string unit_indent = "  ";
  std::string s = indent + name(t) + ", " + to_string(value(t),threshold) + ", " + label(t) + "\n";
  for (const tree& c : children(t)) {
    s += to_string_impl(c,indent+unit_indent,threshold);
  }
  return s;
}

auto to_string(const tree& t, int threshold) -> std::string {
  return to_string_impl(t,"",threshold);
}
// to_string }


} // cgns
