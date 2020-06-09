#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "cpp_cgns/interop/pycgns_converter.hpp"
#include "cpp_cgns/node_manip.hpp"
#include <iostream>

static PyObject*
py_cpp_cgns_partition_with_boundary_first(PyObject* self, PyObject* args) {
  PyObject* base_pytree;
  if (!PyArg_ParseTuple(args, "O", &base_pytree)) return NULL;
  
  cgns::tree base = cgns::view_as_cpptree(base_pytree);

  cgns::cgns_allocator alloc; // allocates and owns memory
  //cgns::partition_with_boundary_first(base,factory(&alloc));

  pytree_with_transfered_ownership_inplace(base,alloc,base_pytree);

  Py_INCREF(Py_None); return Py_None;
}

static PyMethodDef SpamMethods[] = {
    {"partition_with_boundary_first",  py_cpp_cgns_partition_with_boundary_first, METH_VARARGS, "ngon sorted with boundary faces first"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static PyModuleDef py_cpp_cgnsmodule = {
    PyModuleDef_HEAD_INIT,
    "py_cpp_cgns",   /* name of module */
    NULL, /* module documentation, may be NULL */
    -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
    SpamMethods
};

PyMODINIT_FUNC
PyInit_py_cpp_cgns(void)
{
    return PyModule_Create(&py_cpp_cgnsmodule);
}
