#pragma once


#include "cpp_cgns/tree.hpp"
#include "std_e/graph/algorithm/algo_adjacencies.hpp"
#include "std_e/utils/concatenate.hpp"


namespace cgns {


// [Sphinx Doc] Tree manip {
// predicates {
auto is_of_name(const tree& tree, const std::string& name) -> bool;
auto is_of_label(const tree& tree, const std::string& label) -> bool;
auto is_one_of_labels(const tree& tree, const std::vector<std::string>& labels) -> bool;

template<class Unary_pred> auto has_child_by_predicate(const tree& t, Unary_pred p) -> bool;
                           auto has_child_of_name(const tree& t, const std::string& name) -> bool;
                           auto has_child_of_label(const tree& t, const std::string& name) -> bool;
                           auto has_node(const tree& t, const std::string& gen_path) -> bool;
// predicates }


// searches {
template<class Tree, class Unary_pred> auto get_children_by_predicate (Tree& t, Unary_pred p)                          -> Tree_range<Tree>;
template<class Tree, class Unary_pred> auto get_child_by_predicate    (Tree& t, Unary_pred p, const cgns_exception& e) -> tree_ref<Tree>;
template<class Tree, class Unary_pred> auto get_child_by_predicate    (Tree& t, Unary_pred p)                          -> tree_ref<Tree>;

template<class Tree>                   auto get_child_by_name         (Tree& t, const std::string& name ) -> tree_ref<Tree>;

template<class Tree>                   auto get_child_by_label        (Tree& t, const std::string& label) -> tree_ref<Tree>;
template<class Tree>                   auto get_children_by_label     (Tree& t, const std::string& label) -> Tree_range<Tree>;
template<class Tree>                   auto get_children_by_labels    (Tree& t, const std::vector<std::string>& labels) -> Tree_range<Tree>;

template<class Tree>                   auto get_node_by_matching      (Tree& t, const std::string& gen_path) -> Tree&;
template<class Tree>                   auto get_nodes_by_matching     (Tree& t, const std::string& gen_path) -> Tree_range<Tree>;
template<class Tree>                   auto get_nodes_by_matching     (Tree& t, const std::vector<std::string>& gen_paths) -> Tree_range<Tree>;

template<class Tree, class Unary_pred> auto get_node_by_predicate     (Tree& t, Unary_pred p)                -> tree_ref<Tree>;
template<class Tree, class Unary_pred> auto get_nodes_by_predicate    (Tree& t, Unary_pred p)                -> Tree_range<Tree>;

template<class Tree>                   auto get_node_by_name          (Tree& t, const std::string& name )    -> tree_ref<Tree>;
template<class Tree>                   auto get_node_by_label         (Tree& t, const std::string& label)    -> tree_ref<Tree>;
template<class Tree>                   auto get_nodes_by_name         (Tree& t, const std::string& name )    -> Tree_range<Tree>;
template<class Tree>                   auto get_nodes_by_label        (Tree& t, const std::string& label)    -> Tree_range<Tree>;
template<class Tree>                   auto get_nodes_by_labels       (Tree& t, const std::vector<std::string>& label) -> Tree_range<Tree>;

template<class T, int N=1, class Tree> auto get_value                 (Tree& t);
template<class T, int N=1, class Tree> auto get_child_value_by_name   (Tree& t, const std::string& s);
template<class T, int N=1, class Tree> auto get_child_value_by_label  (Tree& t, const std::string& s);
template<class T, int N=1, class Tree> auto get_node_value_by_matching(Tree& t, const std::string& s);
// searches }

// removal {
auto rm_child(tree& t, const tree& c) -> void;
auto rm_child_by_name(tree& t, const std::string& name) -> void;
auto rm_children_by_names(tree& t, const std::vector<std::string>& names) -> void;
auto rm_child_by_label(tree& t, const std::string& label) -> void;
auto rm_children_by_label(tree& t, const std::string& label) -> void;

template<class Tree_range>
auto rm_children(tree& t, Tree_range& children) -> void;

template<class Unary_predicate> auto
rm_child_by_predicate(tree& t, Unary_predicate p, const cgns_exception& e) -> void;
template<class Unary_predicate> auto
rm_child_by_predicate(tree& t, Unary_predicate p) -> void;
template<class Unary_predicate> auto
rm_children_by_predicate(tree& t, Unary_predicate p) -> void;
// removal }

// replace {
auto replace_child(tree& t, tree&& c) -> tree&;
auto replace_children(tree& t, std::vector<tree>&& cs) -> void;
template<class Tree> auto get_child_by_name_or_create(Tree& t, tree&& c) -> Tree&;
// replace }
// [Sphinx Doc] Tree manip }


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
get_child_by_predicate(Tree& t, Unary_pred p, const cgns_exception& e) -> tree_ref<Tree> {
  auto pos = std::find_if(begin(children(t)),end(children(t)),p);
  if (pos==end(children(t))) {
    throw e;
  } else {
    return *pos;
  }
}
template<class Tree, class Unary_pred> auto
get_child_by_predicate(Tree& t, Unary_pred p) -> tree_ref<Tree> {
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
  auto predicate = [&](const tree& child){ return is_one_of_labels(child,labels); };
  return get_children_by_predicate(t,predicate);
}
template<class Tree> auto
get_children_by_name_or_label(Tree& t, const std::string& s) -> Tree_range<Tree> {
  auto predicate = [&](const tree& child){ return is_of_name(child,s) || is_of_label(child,s); };
  return get_children_by_predicate(t,predicate);
}

template<class Tree> auto
get_child_by_name(Tree& t, const std::string& name_) -> tree_ref<Tree> {
  auto predicate = [&](const tree& child){ return is_of_name(child,name_); };
  cgns_exception e("Child of name \""+name_+"\" not found in tree \""+name(t)+"\"");
  return get_child_by_predicate(t,predicate,e);
}
template<class Tree> auto
get_child_by_label(Tree& t, const std::string& label) -> tree_ref<Tree> {
  auto predicate = [&](const tree& child){ return is_of_label(child,label); };
  cgns_exception e("Child of label \""+label+"\" not found in tree \""+name(t)+"\"");
  return get_child_by_predicate(t,predicate,e);
}


//// get_node_by_matching {
template<class Tree>
// requires Tree==tree or Tree==const tree
class visitor_for_matching_path {
  public:
    visitor_for_matching_path(const std::string& gen_path)
      : identifiers(std_e::split(gen_path,'/'))
      , max_depth(identifiers.size()-1)
    {}

    auto
    pre(auto nodes) -> std_e::step {
      const Tree& t = *nodes.back();
      int depth = nodes.size()-1;
      if (depth > max_depth) return std_e::step::over; // continue if gen_path reached the end
      bool is_matching = identifiers[depth]==name(t) || identifiers[depth]==label(t);
      if (!is_matching)                    return std_e::step::over; // prune
      if (is_matching && depth<max_depth)  return std_e::step::into; // continue to match path
      if (is_matching && depth==max_depth) return std_e::step::out ; // found!
      STD_E_ASSERT(0); throw; // unreachable: all cases treated
    }

    auto post(auto&&) {}
    auto up  (auto&&) {}
    auto down(auto&&) {}
  private:
    const std::vector<std::string> identifiers;
    const int max_depth;
};
template<class Tree> auto
get_node_by_matching(Tree& t, const std::string& gen_path) -> Tree& {
  visitor_for_matching_path<Tree> v(gen_path);
  auto res = std_e::depth_first_search_adjacencies(children(t),v,std_e::depth::all);
  if (res == children(t).end()) {
    throw cgns_exception("No sub-tree matching \""+gen_path+"\" in tree \""+name(t)+"\"");
  } else {
    return *res;
  }
}
//// get_node_by_matching }

//// get_nodes_by_matching {
template<class Tree>
// requires Tree==tree or Tree==const tree
class visitor_for_matching_paths : public visitor_for_matching_path<Tree> {
  public:
    using base = visitor_for_matching_path<Tree>;
    using base::base;

    auto
    pre(auto nodes) -> std_e::step {
      auto next_step = base::pre(nodes);
      if (next_step == std_e::step::out) { // found
        Tree& t = *nodes.back();
        matching_nodes.emplace_back(t); // register the node
        next_step = std_e::step::over; // continue search for siblings
      }
      return next_step;
    }

    auto
    retrieve_nodes() -> Tree_range<Tree> {
      return std::move(matching_nodes);
    }
  private:
    Tree_range<Tree> matching_nodes;
};

template<class Tree> auto
get_nodes_by_matching(Tree& t, const std::string& gen_path) -> Tree_range<Tree> {
  visitor_for_matching_paths<Tree> v(gen_path);
  std_e::depth_first_search_adjacencies(children(t),v,std_e::depth::all);
  return v.retrieve_nodes();
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
  auto predicate = [&](auto& child){ return is_one_of_labels(child,labels); };
  return get_nodes_by_predicate(t,predicate);
}
template<class Tree> auto
get_node_by_name(Tree& t, const std::string& s) -> tree_ref<Tree> {
  auto ts = get_nodes_by_name(t,s);
  if (ts.size() == 0) {
    throw cgns_exception("No child of name\""+s+"\" in tree \""+name(t)+"\"");
  } else {
    return ts[0].get();
  }
}
//// get_nodes_by_predicate }


/// common searches }
// tree search }
template<class T, int N> auto
throw_if_incorrect_array_type(const tree& x) -> void {
  if (!cgns::holds_alternative<T>(value(x))) {
    throw cgns_exception("Value of node \""+name(x)+"\" is of type "+value(x).data_type()
                       + " but was asked to be of type "+to_string<T>());
  }
}


// find and give value {
template<class T, int N, class Tree> auto
get_value(Tree& t) {
  throw_if_incorrect_array_type<T,N>(t);
  return view_as_array<T,N>(value(t));
}
template<class T, int N, class Tree> auto
get_child_value_by_name(Tree& t, const std::string& s) {
  auto&& n = get_child_by_name(t,s);
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
rm_child_by_predicate(tree& t, Unary_predicate p, const cgns_exception& e) -> void {
  auto& cs = children(t);
  auto pos = std::find_if(begin(cs),end(cs),p);
  if (pos==end(cs)) {
    throw e;
  } else {
    cs.erase(pos);
  }
}
template<class Unary_predicate> auto
rm_child_by_predicate(tree& t, Unary_predicate p) -> void {
  const cgns_exception e("No child to erase satisfying predicate in tree \""+name(t)+"\"");
  return rm_child_by_predicate(t,p,e);
}
template<class Unary_predicate> auto
rm_children_by_predicate(tree& t, Unary_predicate p) -> void {
  auto& cs = children(t);
  auto not_p = [p](const auto& x){ return !p(x); };
  auto pos = std::stable_partition(begin(cs),end(cs),not_p); // move nodes to be deleted at the end
  cs.erase(pos,end(cs));
}

template<class Tree_range>
auto rm_children(tree& t, Tree_range& children) -> void {
  // deleting from a vector is complicated because of iterator invalidation
  // here we need to register the names, then erase
  std::vector<std::string> child_names(children.size());
  std::transform(begin(children),end(children),begin(child_names),[](const tree& c){ return name(c); });
  for (const auto& name : child_names) {
    rm_child_by_name(t,name);
  }
}
/// node removal }


template<class Tree> auto
get_child_by_name_or_create(Tree& t, tree&& c) -> Tree& {
  if (has_child_of_name(t,name(c))) {
    return get_child_by_name(t,name(c));
  } else {
    return emplace_child(t,std::move(c));
  }
}


} // cgns
