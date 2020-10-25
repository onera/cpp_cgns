#pragma once


//#define PY_SSIZE_T_CLEAN
//#include <Python.h>
#include "cpp_cgns/cgns.hpp"
#include <pybind11/pybind11.h>
namespace py = pybind11;


namespace cgns {

// Fwd decl
class cgns_allocator;


PYBIND11_EXPORT py::object view_as_pytree(tree& t);
PYBIND11_EXPORT tree view_as_cpptree(py::object pytree);

PyObject* pytree_with_transfered_ownership(tree& t, cgns_allocator& alloc);
//void pytree_with_transfered_ownership_inplace(tree& t, cgns_allocator& alloc, PyObject* pytree);
void add_new_nodes_and_ownership(tree& t, cgns_allocator& alloc, PyObject* pytree);
void update_and_transfer_ownership(tree& t, cgns_allocator& alloc, PyObject* pytree);
void update_and_transfer_ownership2(tree& t, cgns_allocator& alloc, PyObject* pytree);

} // cgns
