#pragma once


#include "cpp_cgns/cgns.hpp"
#include "pybind11/pybind11.h"


namespace cgns {

namespace py = pybind11;

// [Sphinx Doc] Python/CGNS <-> C++/CGNS {
PYBIND11_EXPORT auto to_cpp_tree(py::list pytree) -> tree;
PYBIND11_EXPORT auto to_py_tree(tree&& t) -> py::list;

PYBIND11_EXPORT auto update_py_tree(tree&& t, py::list pytree) -> void;

PYBIND11_EXPORT auto view_as_py_tree(tree& t) -> py::list;
PYBIND11_EXPORT auto to_cpp_tree_copy(py::list pytree) -> tree;
// [Sphinx Doc] Python/CGNS <-> C++/CGNS }

// TODO deprecate the name in favor of update_py_tree
PYBIND11_EXPORT auto update_and_transfer_ownership_to_py_tree(tree&& t, py::list pytree) -> void;

} // cgns
