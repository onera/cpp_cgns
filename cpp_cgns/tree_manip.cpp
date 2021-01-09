#include "cpp_cgns/tree_manip.hpp"
#include "cpp_cgns/exception.hpp"
#include "std_e/utils/string.hpp"
#include "std_e/future/contract.hpp"


namespace cgns {


// tree search {


/// common predicates {
bool is_of_name(const tree& tree, const std::string& name) {
  return tree.name == name;
}
bool is_of_label(const tree& tree, const std::string& label) {
  return tree.label == label;
}
bool is_one_of_labels(const tree& tree, const std::vector<std::string>& labels) {
  return std::any_of(begin(labels),end(labels),[&tree](const std::string& label){ return is_of_label(tree,label); });
}

bool has_child_of_name(const tree& t, const std::string& name) {
  auto predicate = [&](const tree& child){ return is_of_name(child,name); };
  return has_child_by_predicate(t,predicate);
}
bool has_child_of_label(const tree& t, const std::string& label) {
  auto predicate = [&](const tree& child){ return is_of_label(child,label); };
  return has_child_by_predicate(t,predicate);
}
/// common predicates }




//// tree comparisons {
//class visitor_for_tree_equality {
//  public:
//    visitor_for_matching_path(const tree& y)
//      , y(y)
//    {}
//
//    auto
//    pre(const tree& x) -> bool {
//      bool is_matching = identifiers[depth]==t.name || identifiers[depth]==t.label;
//      int max_depth = identifiers.size();
//      if (depth==max_depth-1 && is_matching) {
//        matching_nodes.push_back(t);
//      }
//      return depth < max_depth  &&  is_matching;
//    }
//
//    auto
//    post(const tree&) {}
//
//    auto
//    up(const tree&, const tree&) -> void {
//      --depth;
//    }
//    auto
//    down(const tree&, const tree&) -> void {
//      ++depth;
//    }
//
//  private:
//    const tree& y;
//};
//
//auto
//operator==(const tree& x, const tree& y) -> bool {
//  visitor_for_tree_equality v(y);
//  bool end_reached = graph::depth_first_find_adjacencies(x,v) == ;
//  return end_reached;
//}
//
//auto
//operator!=(const tree& x, const tree& y) -> bool {
//  return !(x==y);
//}
//// tree comparisons }


} // cgns
