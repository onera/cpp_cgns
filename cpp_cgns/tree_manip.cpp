#include "cpp_cgns/tree_manip.hpp"

#include "std_e/utils/string.hpp"
#include "std_e/future/contract.hpp"
#include <algorithm>
#include <vector>


namespace cgns {


// tree search {


/// common predicates {
auto
is_of_name(const tree& t, const std::string& s) -> bool {
  return name(t) == s;
}
auto
is_of_label(const tree& t, const std::string& s) -> bool {
  return label(t) == s;
}
auto
is_one_of_labels(const tree& t, const std::vector<std::string>& labels) -> bool {
  return std::any_of(begin(labels),end(labels),[&t](const std::string& label){ return is_of_label(t,label); });
}

auto
has_child_of_name(const tree& t, const std::string& name) -> bool {
  auto predicate = [&](const tree& child){ return is_of_name(child,name); };
  return has_child_by_predicate(t,predicate);
}
auto
has_child_of_label(const tree& t, const std::string& label) -> bool {
  auto predicate = [&](const tree& child){ return is_of_label(child,label); };
  return has_child_by_predicate(t,predicate);
}
/// common predicates }


/// node removal {
auto
rm_child(tree& t, const tree& c) -> void {
  auto predicate = [&](const tree& child){ return &child==&c; };
  rm_child_by_predicate(t,predicate);
}
auto
rm_child_by_name(tree& t, const std::string& name) -> void {
  auto predicate = [&](const tree& child){ return is_of_name(child,name); };
  rm_child_by_predicate(t,predicate);
}
auto rm_children_by_names(tree& t, const std::vector<std::string>& names) -> void {
  for (const auto& name : names) {
    rm_child_by_name(t,name);
  }
}

auto
rm_child_by_label(tree& t, const std::string& label) -> void {
  auto predicate = [&](const tree& child){ return is_of_label(child,label); };
  rm_child_by_predicate(t,predicate);
}
auto
rm_children_by_label(tree& t, const std::string& label) -> void {
  auto predicate = [&](const tree& child){ return is_of_label(child,label); };
  rm_children_by_predicate(t,predicate);
}
/// node removal }


} // cgns
