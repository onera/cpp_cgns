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
    bool same_node =  x.name==y.name  &&  x.label==y.label;
    return !same_node  ||  x.children.size() != y.children.size();
  };
  return !std_e::preorder_depth_first_find_adjacencies_b(xy,different_node);
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
  std::string s = indent + t.name + ", " + to_string(t.value,threshold) + ", " + t.label + "\n";
  for (const auto& c : t.children) {
    s += to_string_impl(c,indent+unit_indent,threshold);
  }
  return s;
}

auto to_string(const tree& t, int threshold) -> std::string {
  return to_string_impl(t,"",threshold);
}
// to_string }


} // cgns
