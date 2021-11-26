#pragma once


#include "cpp_cgns/cgns.hpp"
#include "cpp_cgns/tree_range.hpp"
#include "std_e/graph/algorithm/algo_adjacencies.hpp"
#include "std_e/utils/concatenate.hpp"


namespace cgns {


// predicates {
auto is_of_name(const tree& tree, const std::string& name) -> bool;
auto is_of_label(const tree& tree, const std::string& label) -> bool;
auto is_of_labels(const tree& tree, const std::vector<std::string>& labels) -> bool;

template<class Unary_pred> auto has_child_by_predicate(const tree& t, Unary_pred p) -> bool;
                           auto has_child_of_name(const tree& t, const std::string& name) -> bool;
                           auto has_child_of_label(const tree& t, const std::string& name) -> bool;
                           auto has_node(const tree& t, const std::string& gen_path) -> bool;
// predicates }


// searches {
template<class Tree, class Unary_pred> auto get_children_by_predicate (Tree& t, Unary_pred p)                          -> Tree_range<Tree>;
template<class Tree, class Unary_pred> auto get_child_by_predicate    (Tree& t, Unary_pred p, const cgns_exception& e) -> decltype(auto); // TODO -> Tree&
template<class Tree, class Unary_pred> auto get_child_by_predicate    (Tree& t, Unary_pred p)                          -> decltype(auto);

template<class Tree>                   auto get_child_by_name         (Tree& t, const std::string& name ) -> decltype(auto);

template<class Tree>                   auto get_child_by_label        (Tree& t, const std::string& label) -> decltype(auto);
template<class Tree>                   auto get_children_by_label     (Tree& t, const std::string& label) -> Tree_range<Tree>;
template<class Tree>                   auto get_children_by_labels    (Tree& t, const std::vector<std::string>& labels) -> Tree_range<Tree>;

template<class Tree>                   auto get_node_by_matching      (Tree& t, const std::string& gen_path) -> decltype(auto);
template<class Tree>                   auto get_nodes_by_matching     (Tree& t, const std::string& gen_path) -> Tree_range<Tree>;
template<class Tree>                   auto get_nodes_by_matching     (Tree& t, const std::vector<std::string>& gen_paths) -> Tree_range<Tree>;

template<class Tree, class Unary_pred> auto get_node_by_predicate     (Tree& t, Unary_pred p)                -> decltype(auto);
template<class Tree, class Unary_pred> auto get_nodes_by_predicate    (Tree& t, Unary_pred p)                -> Tree_range<Tree>;

template<class Tree>                   auto get_node_by_name          (Tree& t, const std::string& name )    -> decltype(auto);
template<class Tree>                   auto get_node_by_label         (Tree& t, const std::string& label)    -> decltype(auto);
template<class Tree>                   auto get_nodes_by_name         (Tree& t, const std::string& name )    -> Tree_range<Tree>;
template<class Tree>                   auto get_nodes_by_label        (Tree& t, const std::string& label)    -> Tree_range<Tree>;
template<class Tree>                   auto get_nodes_by_labels       (Tree& t, const std::vector<std::string>& label) -> Tree_range<Tree>;

template<class T, int N=1, class Tree> auto get_child_value_by_name   (Tree& t, const std::string& s);
template<class T, int N=1, class Tree> auto get_child_value_by_label  (Tree& t, const std::string& s);
template<class T, int N=1, class Tree> auto get_node_value_by_matching(Tree& t, const std::string& s);
// searches }

// removal {
auto rm_child(tree& t, const tree& c) -> void;
auto rm_child_by_name(tree& t, const std::string& name) -> void;
auto rm_child_by_label(tree& t, const std::string& label) -> void;
auto rm_children_by_label(tree& t, const std::string& label) -> void;

template<class Tree_range>
auto rm_children(tree& t, Tree_range& children) -> void;

template<class Unary_predicate> auto
rm_children_by_predicate(tree& t, Unary_predicate p) -> void;
template<class Unary_predicate> auto
rm_child_by_predicate(tree& t, Unary_predicate p) -> void;
// removal }




// actions {
//inline auto
//deep_copy(const tree& /*t */, cgns_allocator& /* alloc */) -> tree {
//  throw std_e::not_implemented_exception();
//}
// actions }


// ====================== impl ======================

/// by predicate {
template<class Unary_pred>
bool has_child_by_predicate(const tree& t, Unary_pred p) {
  auto pos = std::find_if(begin(children(t)),end(children(t)),p);
  if (pos==end(children(t))) return false;
  return true;
}

template<class Tree, class Unary_pred> auto
get_children_by_predicate(Tree& t, Unary_pred p) -> Tree_range<Tree>{
  Tree_range<Tree> sub_ts;
  std::copy_if(begin(children(t)),end(children(t)),back_inserter(sub_ts),p);
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
get_children_by_label(Tree& t, const std::string& label) -> Tree_range<Tree> {
  auto predicate = [&](const tree& child){ return is_of_label(child,label); };
  return get_children_by_predicate(t,predicate);
}
template<class Tree> auto
get_children_by_labels(Tree& t, const std::vector<std::string>& labels) -> Tree_range<Tree> {
  auto predicate = [&](const tree& child){ return is_of_labels(child,labels); };
  return get_children_by_predicate(t,predicate);
}
template<class Tree> auto
get_children_by_name_or_label(Tree& t, const std::string& s) -> Tree_range<Tree> {
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
      , max_depth(identifiers.size()-1)
      , depth(0)
    {}

    auto
    pre(Tree& t) -> bool {
      bool is_matching = identifiers[depth]==name(t) || identifiers[depth]==label(t);
      if (is_matching && depth==max_depth) {
        matching_nodes.emplace_back(t);
      }
      return !is_matching || depth > max_depth; // continue if not matching or gen_path reached the end
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
    retrieve_nodes() -> Tree_range<Tree> {
      return std::move(matching_nodes);
    }
  private:
    const std::vector<std::string> identifiers;
    const int max_depth;

    int depth;
    Tree_range<Tree> matching_nodes;
};

template<class Tree> auto
get_nodes_by_matching(Tree& t, const std::string& gen_path) -> Tree_range<Tree> {
  visitor_for_matching_path<Tree> v(name(t)+'/'+gen_path);
  std_e::depth_first_prune_adjacencies(t,v);
  return v.retrieve_nodes();
}
template<class Tree> auto
get_node_by_matching(Tree& t, const std::string& gen_path) -> decltype(auto) {
  // TODO the commented code is bugged (wrong predicate in visitor_for_matching_path.pre)
  //visitor_for_matching_path<Tree> v(name(t)+'/'+gen_path);
  //std_e::depth_first_find_adjacencies(t,v);
  //auto ts = v.retrieve_nodes();
  auto ts = get_nodes_by_matching(t,gen_path);
  if (ts.size() == 0) {
    throw cgns_exception("No sub-tree matching \""+gen_path+"\" in tree \""+name(t)+"\"");
  } else {
    return ts[0];
  }
}
inline auto
has_node(const tree& t, const std::string& gen_path) -> bool {
  auto ts = get_nodes_by_matching(t,gen_path);
  if (ts.size() == 0) {
    return false;
  } else {
    return true;
  }
}
template<class Tree> auto
get_nodes_by_matching(Tree& t, const std::vector<std::string>& gen_paths) -> Tree_range<Tree> {
  Tree_range<Tree> res;
  for (const auto& gen_path : gen_paths) {
    std_e::append(res,get_nodes_by_matching(t,gen_path));
  }
  return res;
}
//// get_nodes_by_matching }


//// get_nodes_by_predicate {
template<class Tree, class Unary_pred> auto
get_nodes_by_predicate(Tree& t, Unary_pred p) -> Tree_range<Tree> {
  Tree_range<Tree> ts;
  auto f = [&ts,p](auto& t){ if (p(t)) ts.emplace_back(t); };
  std_e::preorder_depth_first_scan_adjacencies(t,f);
  return ts;
}
template<class Tree> auto
get_nodes_by_name(Tree& t, const std::string& name) -> Tree_range<Tree> {
  auto predicate = [&](auto& child){ return is_of_name(child,name); };
  return get_nodes_by_predicate(t,predicate);
}
template<class Tree> auto
get_nodes_by_label(Tree& t, const std::string& label) -> Tree_range<Tree> {
  auto predicate = [&](auto& child){ return is_of_label(child,label); };
  return get_nodes_by_predicate(t,predicate);
}
template<class Tree> auto
get_nodes_by_labels(Tree& t, const std::vector<std::string>& labels) -> Tree_range<Tree> {
  auto predicate = [&](auto& child){ return is_of_labels(child,labels); };
  return get_nodes_by_predicate(t,predicate);
}
template<class Tree> auto
get_node_by_name(Tree& t, const std::string& name) -> decltype(auto) {
  auto ts = get_nodes_by_name(t,name);
  if (ts.size() == 0) {
    throw cgns_exception("No child of name\""+name+"\" in tree \""+cgns::name(t)+"\"");
  } else {
    return ts[0].get();
  }
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


/// node removal {
template<class Unary_predicate> auto
rm_child_by_predicate(tree& t, Unary_predicate p) -> void {
  auto& cs = t.children;
  auto pos = std::find_if(begin(cs),end(cs),p);
  if (pos==end(cs)) {
    throw cgns_exception("No node to erase");
  } else {
    cs.erase(pos);
  }
}
template<class Unary_predicate> auto
rm_children_by_predicate(tree& t, Unary_predicate p) -> void {
  auto& cs = t.children;
  auto not_p = [p](const auto& x){ return !p(x); };
  auto pos = std::stable_partition(begin(cs),end(cs),not_p); // move nodes to be deleted at the end
  cs.erase(pos,end(cs));
}

template<class Tree_range>
auto rm_children(tree& t, Tree_range& children) -> void {
  // deleting from a vector is complicated because of iterator invalidation
  // here we need to register the names, then erase
  std::vector<std::string> child_names(children.size());
  std::transform(begin(children),end(children),begin(child_names),[](const tree& c){ return c.name; });
  for (const auto& name : child_names) {
    rm_child_by_name(t,name);
  }
}
/// node removal }


} // cgns
