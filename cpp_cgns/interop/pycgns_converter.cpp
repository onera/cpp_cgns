#include "cpp_cgns/interop/pycgns_converter.hpp"

#include <algorithm>
#include "pybind11/numpy.h"
#include "std_e/future/contract.hpp"

#include "cpp_cgns/cgns.hpp"
#include "cpp_cgns/exception.hpp"
#include "cpp_cgns/allocator.hpp"


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


// data_type <-> numpy type {
struct cgns_numpy_type {
  std::string cgns_type;
  std::string np_type;
};
const std::vector<cgns_numpy_type> cgns_numpy_types = {
  {"C1" , "int8"},
  {"I4" , "int32"},
  {"I8" , "int64"},
  {"R4" , "f4"},
  {"R8" , "f8"},
};

auto
numpy_type_to_cgns_type(py::dtype type) -> std::string {
  auto np_type = std::string(py::str(type));
  auto matching_numpy_type = [np_type](auto x){ return x.np_type==np_type; };
  auto pos = std::find_if(begin(cgns_numpy_types),end(cgns_numpy_types),matching_numpy_type);
  if (pos==end(cgns_numpy_types)) {
    throw cgns_exception("Unknown numpy type \""+np_type+"\"");
  } else {
    return pos->cgns_type;
  }
}
auto
cgns_type_to_numpy_type(const std::string& cgns_type) -> std::string {
  auto matching_data_type = [&cgns_type](auto x){ return x.cgns_type==cgns_type; };
  auto pos = std::find_if(begin(cgns_numpy_types),end(cgns_numpy_types),matching_data_type);
  if (pos==end(cgns_numpy_types)) {
    throw cgns_exception("Unknown cgns data type \""+cgns_type+"\"");
  } else {
    return pos->np_type;
  }
}
// data_type <-> numpy type }


// py value <-> node_value {
/// py value -> node_value {
auto
view_as_node_value(py::array np_arr) -> node_value {
  if (!(np_arr.flags() & py::array::f_style)) {
    throw cgns_exception("In python/CGNS, numpy array must be contiguous and fortran-ordered");
  }

  auto type = np_arr.dtype();
  auto data_type = numpy_type_to_cgns_type(type);

  auto n_dim = np_arr.ndim();

  auto dims_ptr = np_arr.shape();
  std::vector<I8> dims(n_dim);
  std::copy_n(dims_ptr,n_dim,begin(dims));

  // TODO
  //ELOG(np_arr.writeable());
  //void* data = np_arr.mutable_data();
  void* data = (void*)np_arr.data();
  return {data_type,dims,data};
}
auto
to_node_value(py::object value) -> node_value {
  if (value.is_none()) {
    return MT;
  } else {
    return view_as_node_value(value);
  }
}
/// py value -> node_value }

/// node_value -> py value {
auto
to_np_array(node_value& n, py::handle capsule = py::handle()) {
  auto np_type = cgns_type_to_numpy_type(n.data_type);
  auto dt = py::dtype(np_type);
  auto strides = py::detail::f_strides(n.dims, dt.itemsize());
  return py::array(dt,n.dims,strides,n.data,capsule);
}

auto
capsule_to_free_node_value_memory(node_value& n, cgns_allocator& alloc) {
  // SEE https://stackoverflow.com/a/52737023/1583122
  // SEE https://stackoverflow.com/a/44682603/1583122
  return py::capsule(n.data, alloc.memory_destructor_function());
}
auto
to_owning_np_array(node_value& n, cgns_allocator& alloc) {
  auto capsule = capsule_to_free_node_value_memory(n,alloc);
  return to_np_array(n,capsule);
}

auto
to_py_value(node_value& value) -> py::object { // NOTE: non const ref because shared data
  if (value.data_type=="MT") {
    return py::none();
  } else {
    return to_np_array(value);
  }
}
auto
to_owning_py_value(node_value& val, cgns_allocator& alloc) -> py::object { // NOTE: non const ref because shared data
  if (val.data_type=="MT") {
    return py::none();
  } else {
    return to_owning_np_array(val,alloc);
  }
}
/// node_value -> py value }
// py value <-> node_value }


// tree <-> py_tree {
/// no ownership transfer {
auto
view_as_cpptree(py::list py_tree) -> tree {
  std::string  name_ = to_string_from_py(name (py_tree));
  std::string label_ = to_string_from_py(label(py_tree));
  node_value  value_ = to_node_value    (value(py_tree));

  py::list py_children = children(py_tree);
  int n_child = py_children.size();
  std::vector<tree> children_(n_child);
  for (int i=0; i<n_child; ++i) {
    py::list py_child = py_children[i];
    children_[i] = view_as_cpptree(py_child);
  }

  return {name_,label_,value_,children_};
}

auto
view_as_pytree(tree& t) -> py::list {
  auto py_tree = new_py_tree();

  name (py_tree) = name (t);
  label(py_tree) = label(t);
  value(py_tree) = to_py_value(value(t));

  int n_child = t.children.size();
  py::list py_children(n_child);
  for (int i=0; i<n_child; ++i) {
    py::object py_child = view_as_pytree(t.children[i]);
    py_children[i] = py_child;
  }
  children(py_tree) = py_children;

  return py_tree;
}
/// no ownership transfer }


/// ownership transfer to python {
auto
to_np_array_with_transfered_ownership(node_value& n, cgns_allocator& alloc) {
  if (alloc.release_if_owner(n.data)) {
    return to_owning_py_value(n,alloc);
  } else { // case where memory was allocated by another allocator
    return to_py_value(n); // can't transfer since we don't know the allocator
  }
}

auto
pytree_with_transfered_ownership(tree& t, cgns_allocator& alloc) -> py::list {
  // creates a py_tree from t
  // if t memory was allocated with alloc,
  //   then we release the ownship from alloc
  //   and create a numpy array owning it (PyArray_SimpleNewFromData && cleanup capsule)
  // if not, we only create a non-owning numpy array (PyArray_SimpleNewFromData)
  auto py_tree = new_py_tree();

  name (py_tree) = name (t);
  label(py_tree) = label(t);
  value(py_tree) = to_np_array_with_transfered_ownership(value(t),alloc);

  int n_child = t.children.size();
  py::list py_children(n_child);
  for (int i=0; i<n_child; ++i) {
    auto py_child = pytree_with_transfered_ownership(t.children[i],alloc);
    py_children[i] = py_child;
  }
  children(py_tree) = py_children;

  return py_tree;
}

auto
update_and_transfer_ownership2(tree& t, cgns_allocator& alloc, py::list py_tree) -> void {
  // preconditions:
  //   - nodes similar in t and py_tree have the same order
  //     (in other words, t nodes were never reordered)
  //   - nodes are identified by name (no renaming in t)
  STD_E_ASSERT(name (t)==name (py_tree));
  STD_E_ASSERT(label(t)==label(py_tree));

  bool node_data_was_coming_from_python = same_node_data(value(t),to_node_value(value(py_tree)));
  if (!node_data_was_coming_from_python) {
    value(py_tree) = to_np_array_with_transfered_ownership(value(t),alloc);
  }

  py::list py_children = children(py_tree);
  int py_n_child = py_children.size();
  int n_child = t.children.size();
  int i = 0;
  int i_py = 0;
  while (i_py < py_n_child) {
    auto py_child = py_children[i_py];
    if (i<n_child && name(t.children[i])==to_string_from_py(name(py_child))) {
      update_and_transfer_ownership2(t.children[i],alloc,py_child);
      ++i;
      ++i_py;
    } else { // since t and py_tree have the same order
             // it means an item has been removed in t
             // and this should be propagated to py_tree
      PyList_SetSlice(py_children.ptr(), i_py, i_py+1, NULL); // rm item at i_py TODO pybind API
    }
  }
  for (; i<n_child; ++i) {
    auto py_child = pytree_with_transfered_ownership(t.children[i],alloc);
    py_children.append(py_child);
  }
}
/// ownership transfer to python }
// tree <-> py_tree }


} // cgns
