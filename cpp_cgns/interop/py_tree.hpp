#pragma once


//#include "cpp_cgns/cgns.hpp"
#include "pybind11/pybind11.h"


namespace cgns {

namespace py = pybind11;

PYBIND11_EXPORT auto to_cpp_tree2(py::list ptree) -> tree;

PYBIND11_EXPORT auto to_py_tree2(tree& t) -> py::list;

} // cgns
