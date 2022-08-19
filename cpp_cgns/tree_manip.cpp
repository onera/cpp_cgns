#if __cplusplus > 201703L
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
auto
has_node(const tree& t, const std::string& gen_path) -> bool {
  auto ts = get_nodes_by_matching(t,gen_path);
  if (ts.size() == 0) {
    return false;
  } else {
    return true;
  }
}
/// common predicates }


/// node removal {
auto
rm_child(tree& t, const tree& c) -> void {
  auto predicate = [&](const tree& child){ return &child==&c; };
  const cgns_exception e("Impossible to erase child "+name(c)+": it is not a child of tree "+name(t));
  rm_child_by_predicate(t,predicate,e);
}
auto
rm_child_by_name(tree& t, const std::string& name_) -> void {
  auto predicate = [&](const tree& child){ return is_of_name(child,name_); };
  const cgns_exception e("Impossible to erase child of name "+name_+" in tree "+name(t)+": no such child with such name");
  rm_child_by_predicate(t,predicate,e);
}
auto rm_children_by_names(tree& t, const std::vector<std::string>& names) -> void {
  for (const auto& name : names) {
    rm_child_by_name(t,name);
  }
}

auto
rm_child_by_label(tree& t, const std::string& label) -> void {
  auto predicate = [&](const tree& child){ return is_of_label(child,label); };
  const cgns_exception e("Impossible to erase child of label "+label+" in tree "+name(t)+": no such child with such label");
  rm_child_by_predicate(t,predicate,e);
}
auto
rm_children_by_label(tree& t, const std::string& label) -> void {
  auto predicate = [&](const tree& child){ return is_of_label(child,label); };
  rm_children_by_predicate(t,predicate);
}
/// node removal }


auto
replace_child(tree& t, tree&& c) -> tree& {
  if (has_child_of_name(t,name(c))) {
    rm_child_by_name(t,name(c));
  }
  return emplace_child(t,std::move(c));
}
auto
replace_children(tree& t, std::vector<tree>&& cs) -> void {
  for (size_t i=0; i<cs.size(); ++i) {
    replace_child(t,std::move(cs[i]));
  }
}


} // cgns
#endif // C++>17
