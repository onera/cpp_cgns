#pragma once


#include "cpp_cgns/cgns.hpp"
#include "pybind11/pybind11.h"


namespace cgns {

namespace py = pybind11;

PYBIND11_EXPORT auto to_cpp_tree(py::list pytree) -> tree;

PYBIND11_EXPORT auto to_py_tree(tree& t) -> py::list;
PYBIND11_EXPORT auto to_owning_py_tree(tree& t) -> py::list;

PYBIND11_EXPORT auto update_and_transfer_ownership_to_py_tree(tree& t, py::list pytree) -> void;

} // cgns
