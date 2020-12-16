#pragma once


#include "cpp_cgns/cgns.hpp"
#include "cpp_cgns/node_manip.hpp"
#include "cpp_cgns/exception.hpp"
#include "std_e/graph/algorithm/algo_nodes.hpp"


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

template<class Tree, class Unary_pred> auto get_node_by_predicate     (Tree& t, Unary_pred p)                -> decltype(auto);
template<class Tree, class Unary_pred> auto get_nodes_by_predicate    (Tree& t, Unary_pred p)                -> range_of_ref<Tree>;

template<class Tree>                   auto get_node_by_name          (Tree& t, const std::string& name )    -> decltype(auto);
template<class Tree>                   auto get_node_by_label         (Tree& t, const std::string& label)    -> decltype(auto);
template<class Tree>                   auto get_nodes_by_name         (Tree& t, const std::string& name )    -> range_of_ref<Tree>;
template<class Tree>                   auto get_nodes_by_label        (Tree& t, const std::string& label)    -> range_of_ref<Tree>;

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


//// get_nodes_by_matching {
template<class Tree>
// requires Tree==tree or Tree==const tree
class visitor_for_matching_path {
  public:
    visitor_for_matching_path(const std::string& gen_path)
      : identifiers(std_e::split(gen_path,'/'))
      , depth(0)
    {}

    auto
    pre(Tree& t) -> bool {
      bool is_matching = identifiers[depth]==t.name || identifiers[depth]==t.label;
      int max_depth = identifiers.size();
      if (depth==max_depth-1 && is_matching) {
        matching_nodes.push_back(t);
      }
      return depth < max_depth  &&  is_matching;
    }

    auto
    post(Tree&) {}

    auto
    up(Tree&, Tree&) -> void {
      --depth;
    }
    auto
    down(Tree&, Tree&) -> void {
      ++depth;
    }

    auto
    retrieve_nodes() -> range_of_ref<Tree> {
      return std::move(matching_nodes);
    }
  private:
    std::vector<std::string> identifiers;
    int depth;
    range_of_ref<Tree> matching_nodes;
};

template<class Tree> auto
get_nodes_by_matching(Tree& t, const std::string& gen_path) -> range_of_ref<Tree> {
  visitor_for_matching_path<Tree> v(name(t)+'/'+gen_path);
  graph::depth_first_prune_adjacencies(t,v);
  return v.retrieve_nodes();
}
template<class Tree> auto
get_node_by_matching(Tree& t, const std::string& gen_path) -> decltype(auto) {
  visitor_for_matching_path<Tree> v(name(t)+'/'+gen_path);
  graph::depth_first_find_adjacencies(t,v);
  auto ts = v.retrieve_nodes();
  if (ts.size() == 0) {
    throw cgns_exception("No sub-tree matching \""+gen_path+"\" in tree \""+name(t)+"\"");
  } else {
    return ts[0].get();
  }
}

template<class Array0, class Array1> constexpr auto
append(Array0& x, const Array1& y)  {
  for (const auto& e : y) {
    x.push_back(e);
  }
}
template<class Tree> auto
get_nodes_by_matching(Tree& t, const std::vector<std::string>& gen_paths) -> range_of_ref<Tree> {
  range_of_ref<Tree> res;
  for (const auto& gen_path : gen_paths) {
    append(res,get_nodes_by_matching(t,gen_path));
  }
  return res;
}
//// get_nodes_by_matching }


//// get_nodes_by_predicate {
template<class Tree, class Unary_pred> auto
get_nodes_by_predicate(Tree& t, Unary_pred p) -> range_of_ref<Tree> {
  range_of_ref<Tree> ts;
  auto f = [&ts,p](auto& t){ if (p(t)) ts.push_back(t); };
  graph::preorder_depth_first_scan_adjacencies(t,f);
  return ts;
}
template<class Tree> auto
get_nodes_by_name(Tree& t, const std::string& name) -> range_of_ref<Tree> {
  auto predicate = [&](auto& child){ return is_of_name(child,name); };
  return get_nodes_by_predicate(t,predicate);
}
template<class Tree> auto
get_nodes_by_label(Tree& t, const std::string& label) -> range_of_ref<Tree> {
  auto predicate = [&](auto& child){ return is_of_label(child,label); };
  return get_nodes_by_predicate(t,predicate);
}
//// get_nodes_by_predicate }


/// common searches }
// tree search }
template<class T, int N, class Tree> auto
throw_if_incorrect_array_type(const Tree& x) -> void {
  if (value(x).data_type!=to_string<T>()) {
    throw cgns_exception("Value of node \""+name(x)+"\" is of type "+value(x).data_type
                       + " but was asked to be of type "+to_string<T>());
  }
  if (N!=std_e::dynamic_size && int(value(x).dims.size())!=N) {
    throw cgns_exception("Array rank of node \""+name(x)+"\" is "+std::to_string(value(x).dims.size())
                       + " but was asked to be "+std::to_string(N));
  }
}


// find and give value {
template<class T, int N, class Tree> auto
get_child_value_by_name(Tree& t, const std::string& s) {
  Tree& n = get_child_by_name(t,s);
  throw_if_incorrect_array_type<T,N>(n);
  return view_as_array<T,N>(value(n));
}
template<class T, int N, class Tree> auto
get_child_value_by_label(Tree& t, const std::string& s) {
  Tree& n = get_child_by_label(t,s);
  throw_if_incorrect_array_type<T,N>(n);
  return view_as_array<T,N>(value(n));
}
template<class T, int N, class Tree> auto
get_node_value_by_matching(Tree& t, const std::string& s) {
  Tree& n = get_node_by_matching(t,s);
  throw_if_incorrect_array_type<T,N>(n);
  return view_as_array<T,N>(value(n));
}
// find and give value }


} // cgns
