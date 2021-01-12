#pragma once


#include "cpp_cgns/cgns.hpp"
#include "pybind11/numpy.h"


namespace cgns {


namespace py = pybind11;

PYBIND11_EXPORT auto view_as_node_value(py::array np_arr) -> node_value;

PYBIND11_EXPORT auto to_np_array(node_value& n, py::handle capsule = {}) -> py::array;


} // cgns
