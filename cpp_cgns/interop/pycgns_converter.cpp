#include "cpp_cgns/interop/pycgns_converter.hpp"

#include <algorithm>
#include "std_e/future/contract.hpp"

#include "cpp_cgns/interop/node_value_conversion.hpp"
#include "std_e/multi_index/cartesian_product_size.hpp"
#include "std_e/log.hpp"


namespace cgns {


// utility {
auto
to_string_from_py(py::object x) -> std::string {
  return py::str(x);
}
// utility }


// Python/CGNS tree {
auto new_py_tree() {
  return py::list(4);
}
auto name(py::list t) {
  return t[0];
}
auto value(py::list t) {
  return t[1];
}
auto children(py::list t) {
  return t[2];
}
auto label(py::list t) {
  return t[3];
}
// Python/CGNS tree }


// py value <-> node_value {
auto
to_node_value(py::object value) -> node_value {
  if (value.is_none()) {
    return MT();
  } else if (py::isinstance<py::str>(value)) {
    return copy_py_string_to_node_value(value);
  } else {
    return view_as_node_value(value);
  }
}
auto
to_node_value_copy(py::object value) -> node_value {
  if (value.is_none()) {
    return MT();
  } else if (py::isinstance<py::str>(value)) {
    return copy_py_string_to_node_value(value);
  } else {
    return copy_to_node_value(value);
  }
}

auto
to_py_value(node_value& value) -> py::object {
  if (value.data_type()=="MT") {
    return py::none{};
  } else {
    return to_np_array(value);
  }
}
auto
to_owning_py_value(node_value&& value) -> py::object {
  if (value.data_type()=="MT") {
    return py::none{};
  } else if (std_e::cartesian_product_size(value.extent())==0) {
    return to_empty_np_array(value.data_type());
  } else {
    return to_owning_np_array(std::move(value));
  }
}
// py value <-> node_value }


// tree <-> py_tree {
/// no ownership transfer {
auto
to_cpp_tree(py::list py_tree) -> tree {
  std::string  name_ = to_string_from_py(name (py_tree));
  std::string label_ = to_string_from_py(label(py_tree));
  node_value  value_ = to_node_value    (value(py_tree));

  py::list py_children = children(py_tree);
  int n_child = py_children.size();
  std::vector<tree> children_(n_child);
  for (int i=0; i<n_child; ++i) {
    py::list py_child = py_children[i];
    children_[i] = to_cpp_tree(py_child);
  }

  return {name_,label_,std::move(value_),std::move(children_)};
}
auto
to_cpp_tree_copy(py::list py_tree) -> tree {
  std::string  name_ = to_string_from_py (name (py_tree));
  std::string label_ = to_string_from_py (label(py_tree));
  node_value  value_ = to_node_value_copy(value(py_tree));

  py::list py_children = children(py_tree);
  int n_child = py_children.size();
  std::vector<tree> children_(n_child);
  for (int i=0; i<n_child; ++i) {
    py::list py_child = py_children[i];
    children_[i] = to_cpp_tree_copy(py_child);
  }

  return {name_,label_,std::move(value_),std::move(children_)};
}

auto
to_py_tree(tree& t) -> py::list {
  auto py_tree = new_py_tree();

  name (py_tree) = name (t);
  label(py_tree) = label(t);
  value(py_tree) = to_py_value(value(t));

  int n_child = t.children.size();
  py::list py_children(n_child);
  for (int i=0; i<n_child; ++i) {
    py::object py_child = to_py_tree(t.children[i]);
    py_children[i] = py_child;
  }
  children(py_tree) = py_children;

  return py_tree;
}
/// no ownership transfer }


/// ownership transfer to python {
auto
to_owning_py_tree(tree&& t) -> py::list {
  // creates a py_tree from t
  // each owner node has its ownership transfered to Python (capsule mechanism)
  auto py_tree = new_py_tree();

  name (py_tree) = name (t);
  label(py_tree) = label(t);
  value(py_tree) = to_owning_py_value(std::move(t.value));

  int n_child = t.children.size();
  py::list py_children(n_child);
  for (int i=0; i<n_child; ++i) {
    auto py_child = to_owning_py_tree(std::move(t.children[i]));
    py_children[i] = py_child;
  }
  children(py_tree) = py_children;

  return py_tree;
}

auto
update_and_transfer_ownership_to_py_tree(tree&& t, py::list py_tree) -> void {
  // preconditions:
  //   - nodes similar in t and py_tree have the same order
  //     (in other words, t nodes were never reordered)
  //   - nodes are identified by name (no renaming in t)
  STD_E_ASSERT(name (t)==to_string_from_py(name (py_tree)));
  STD_E_ASSERT(label(t)==to_string_from_py(label(py_tree)));

  bool node_data_was_coming_from_python = same_data(value(t),to_node_value(value(py_tree)));
  if (!node_data_was_coming_from_python) {
    value(py_tree) = to_owning_py_value(std::move(t.value));
  }

  py::list py_children = children(py_tree);
  int n_child = t.children.size();
  int i = 0;
  int i_py = 0;
  while (i_py < (int)py_children.size()) { // NOTE: py_children.size() updated in the body of the loop, so do not store before
    auto py_child = py_children[i_py];
    if (i<n_child && name(t.children[i])==to_string_from_py(name(py_child))) {
      update_and_transfer_ownership_to_py_tree(std::move(t.children[i]),py_child);
      ++i;
      ++i_py;
    } else { // since t and py_tree have the same order
             // it means an item has been removed in t
             // and this should be propagated to py_tree
      py_children.attr("pop")(i_py);
    }
  }
  for (; i<n_child; ++i) {
    auto py_child = to_owning_py_tree(std::move(t.children[i]));
    py_children.append(py_child);
  }
}
/// ownership transfer to python }
// tree <-> py_tree }


} // cgns
