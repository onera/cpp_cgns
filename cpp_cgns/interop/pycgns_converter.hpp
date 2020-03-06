#pragma once


#include <Python.h>
#include "cpp_cgns/cpp_cgns.hpp"

// TODO CHECK why this is needed
#if PY_VERSION_HEX >= 0x03000000
#define PyString_FromString PyBytes_FromString
#define PyString_AsString   PyBytes_AsString
#define PyString_Check      PyBytes_Check
#define PyInt_Check         PyLong_Check
#define PyInt_AsLong        PyLong_AsLong
#define PyInt_FromLong      PyLong_FromLong
#endif

namespace cpp_cgns {


PyObject* view_as_pytree(tree& t);
tree view_as_cpptree(PyObject* pytree);


} // cpp_cgns
