#pragma once


#include "cpp_cgns/cgns.hpp"
#include "pybind11/pybind11.h"


namespace cgns {


namespace py = pybind11;

// Fwd decl
class cgns_allocator;


// TODO RENAME
//to_cpp_tree
//to_py_tree
//to_owning_py_tree
//update_owning_py_tree

PYBIND11_EXPORT auto view_as_cpptree(py::list pytree) -> tree;

PYBIND11_EXPORT auto view_as_pytree(tree& t) -> py::list;
PYBIND11_EXPORT auto pytree_with_transfered_ownership(tree& t, cgns_allocator& alloc) -> py::list;

PYBIND11_EXPORT auto update_and_transfer_ownership2(tree& t, cgns_allocator& alloc, py::list pytree) -> void;

} // cgns
