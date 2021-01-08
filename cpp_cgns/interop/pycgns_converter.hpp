#pragma once


//#define PY_SSIZE_T_CLEAN
//#include <Python.h>
#include "cpp_cgns/cgns.hpp"
#include <pybind11/pybind11.h>
namespace py = pybind11;


namespace cgns {

// Fwd decl
class cgns_allocator;


// TODO RENAME
//to_cpp_tree
//to_py_tree
//to_owning_py_tree
//update_owning_py_tree

PYBIND11_EXPORT tree view_as_cpptree(py::list pytree);

PYBIND11_EXPORT py::object view_as_pytree(tree& t);
PyObject* pytree_with_transfered_ownership(tree& t, cgns_allocator& alloc);
//void add_new_nodes_and_ownership(tree& t, cgns_allocator& alloc, PyObject* pytree);
//void update_and_transfer_ownership(tree& t, cgns_allocator& alloc, PyObject* pytree);

void update_and_transfer_ownership2(tree& t, cgns_allocator& alloc, PyObject* pytree);

} // cgns
