#pragma once


#include "cpp_cgns/cgns.hpp"
#include "cpp_cgns/node_manip.hpp"
#include "cpp_cgns/exception.hpp"


namespace cgns {


// predicates {
auto is_of_name(const tree& tree, const std::string& name) -> bool;
auto is_of_label(const tree& tree, const std::string& label) -> bool;
auto is_one_of_labels(const tree& tree, const std::vector<std::string>& labels) -> bool;

template<class Unary_pred> auto has_child_by_predicate(const tree& t, Unary_pred p) -> bool;
                           auto has_child_of_name(const tree& t, const std::string& name) -> bool;
                           auto has_child_of_label(const tree& t, const std::string& name) -> bool;
// predicates }


// searches {
template<class Tree, class Unary_pred> auto get_children_by_predicate (Tree& t, Unary_pred p)                          -> range_of_ref<Tree>;
template<class Tree, class Unary_pred> auto get_child_by_predicate    (Tree& t, Unary_pred p, const cgns_exception& e) -> decltype(auto);
template<class Tree, class Unary_pred> auto get_child_by_predicate    (Tree& t, Unary_pred p)                          -> decltype(auto);

template<class Tree>                   auto get_child_by_name         (Tree& t, const std::string& name ) -> decltype(auto);

template<class Tree>                   auto get_child_by_label        (Tree& t, const std::string& label) -> decltype(auto);
template<class Tree>                   auto get_children_by_label     (Tree& t, const std::string& label) -> range_of_ref<Tree>;
template<class Tree>                   auto get_children_by_labels    (Tree& t, const std::vector<std::string>& labels) -> range_of_ref<Tree>;

template<class Tree>                   auto get_node_by_matching      (Tree& t, const std::string& gen_path) -> decltype(auto);
template<class Tree>                   auto get_nodes_by_matching     (Tree& t, const std::string& gen_path) -> range_of_ref<Tree>;
template<class Tree>                   auto get_nodes_by_matching     (Tree& t, const std::vector<std::string>& gen_paths) -> range_of_ref<Tree>;

template<class T, int N=1, class Tree> auto get_child_value_by_name   (Tree& t, const std::string& s);
template<class T, int N=1, class Tree> auto get_child_value_by_label  (Tree& t, const std::string& s);
template<class T, int N=1, class Tree> auto get_node_value_by_matching(Tree& t, const std::string& s);
// searches }


// ====================== impl ======================

/// by predicate {
template<class Unary_pred>
bool has_child_by_predicate(const tree& t, Unary_pred p) {
  auto pos = std::find_if(begin(children(t)),end(children(t)),p);
  if (pos==end(children(t))) return false;
  return true;
}

template<class Tree, class Unary_pred> auto
get_children_by_predicate(Tree& t, Unary_pred p) -> range_of_ref<Tree>{
  range_of_ref<Tree> sub_ts;
  std::copy_if(begin(children(t)),end(children(t)),std::back_inserter(sub_ts),p);
  return sub_ts;
}
template<class Tree, class Unary_pred> auto
get_child_by_predicate(Tree& t, Unary_pred p, const cgns_exception& e) -> decltype(auto) {
  auto pos = std::find_if(begin(children(t)),end(children(t)),p);
  if (pos==end(children(t))) {
    throw e;
  } else {
    return *pos;
  }
}
template<class Tree, class Unary_pred> auto
get_child_by_predicate(Tree& t, Unary_pred p) -> decltype(auto) {
  const cgns_exception e("No child satisfying predicate found in tree \""+name(t)+"\"");
  return get_child_by_predicate(t,p,e);
}
/// by predicate }


/// common searches {
template<class Tree> auto
get_children_by_label(Tree& t, const std::string& label) -> range_of_ref<Tree> {
  auto predicate = [&](const tree& child){ return is_of_label(child,label); };
  return get_children_by_predicate(t,predicate);
}
template<class Tree> auto
get_children_by_labels(Tree& t, const std::vector<std::string>& labels) -> range_of_ref<Tree> {
  auto predicate = [&](const tree& child){ return is_one_of_labels(child,labels); };
  return get_children_by_predicate(t,predicate);
}
template<class Tree> auto
get_children_by_name_or_label(Tree& t, const std::string& s) -> range_of_ref<Tree> {
  auto predicate = [&](const tree& child){ return is_of_name(child,s) || is_of_label(child,s); };
  return get_children_by_predicate(t,predicate);
}

template<class Tree> auto
get_child_by_name(Tree& t, const std::string& name_) -> decltype(auto) {
  auto predicate = [&](const tree& child){ return is_of_name(child,name_); };
  cgns_exception e("Child of name \""+name_+"\" not found in tree \""+name(t)+"\"");
  return get_child_by_predicate(t,predicate,e);
}
template<class Tree> auto
get_child_by_label(Tree& t, const std::string& label) -> decltype(auto) {
  auto predicate = [&](const tree& child){ return is_of_label(child,label); };
  cgns_exception e("Child of label \""+label+"\" not found in tree \""+name(t)+"\"");
  return get_child_by_predicate(t,predicate,e);
}

// TODO get_nodes_by_matching: this implementation is ugly and slow (memory allocations and copies all the way) and should be replaced by a proper DFS
template<class Array0, class Array1> constexpr auto
append(Array0& x, const Array1& y)  {
  for (const auto& e : y) {
    x.push_back(e);
  }
}
template<class Tree> auto
get_nodes_by_matching_impl(Tree& t, std::vector<std::string> identifiers_stack) -> range_of_ref<Tree> {
  STD_E_ASSERT(identifiers_stack.size()>0);

  auto current_id = identifiers_stack.back();
  range_of_ref<Tree> nodes_matching_current_id = get_children_by_name_or_label(t,current_id);

  if (identifiers_stack.size()==1) {
    return nodes_matching_current_id;
  } else {
    identifiers_stack.pop_back();

    range_of_ref<Tree> matching_nodes;
    for (Tree& node : nodes_matching_current_id) {
      append(matching_nodes,get_nodes_by_matching_impl(node,identifiers_stack));
    }
    return matching_nodes;
  }
}

template<class Tree> auto
get_nodes_by_matching(Tree& t, const std::string& gen_path) -> range_of_ref<Tree> {
  auto identifiers = std_e::split(gen_path,'/');
  std::reverse(begin(identifiers),end(identifiers));
  return get_nodes_by_matching_impl(t,identifiers);
}
template<class Tree> auto
get_nodes_by_matching(Tree& t, const std::vector<std::string>& gen_paths) -> range_of_ref<Tree> {
  range_of_ref<Tree> res;
  for (const auto& gen_path : gen_paths) {
    append(res,get_nodes_by_matching(t,gen_path));
  }
  return res;
}

template<class Tree> auto
get_node_by_matching(Tree& t, const std::string& gen_path) -> decltype(auto) {
  range_of_ref<Tree> ts = get_nodes_by_matching(t,gen_path);
  if (ts.size() == 0) {
    throw cgns_exception("No sub-tree matching \""+gen_path+"\" in tree \""+name(t)+"\"");
  } else {
    return ts[0].get();
  }
}
/// common searches }
// tree search }

template<class T, int N, class Tree> auto
get_child_value_by_name(Tree& t, const std::string& s) {
  Tree& n = get_child_by_name(t,s);
  return view_as_array<T,N>(value(n));
}
template<class T, int N, class Tree> auto
get_child_value_by_label(Tree& t, const std::string& s) {
  Tree& n = get_child_by_label(t,s);
  return view_as_array<T,N>(value(n));
}
template<class T, int N, class Tree> auto
get_node_value_by_matching(Tree& t, const std::string& s) {
  Tree& n = get_node_by_matching(t,s);
  return view_as_array<T,N>(value(n));
}


} // cgns
