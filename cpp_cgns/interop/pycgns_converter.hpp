#pragma once

#include <Python.h>
#include "cpp_cgns/cgns.hpp"

namespace cgns {

PyObject* view_as_pytree(tree& t);
tree view_as_cpptree(PyObject* pytree);

} // cgns
