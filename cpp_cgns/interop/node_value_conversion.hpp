#pragma once


#include "cpp_cgns/cgns.hpp"
#include "pybind11/numpy.h"


namespace cgns {


namespace py = pybind11;

PYBIND11_EXPORT auto view_as_node_value(py::array np_arr) -> node_value;
PYBIND11_EXPORT auto view_py_string_as_node_value(py::object str) -> node_value;

// Turn a node_value into a numpy array
// The data is not copied, and by default (no capule passed), no ownership is transfered
// Note:
//   The default capsule parameter is py::none()
//   Else pybind allocates an array and copies the data
//   SEE https://github.com/pybind/pybind11/issues/2533
PYBIND11_EXPORT auto to_np_array(node_value& n, py::handle capsule = py::none()) -> py::array;

PYBIND11_EXPORT auto to_empty_np_array(node_value& n) -> py::array;


} // cgns
