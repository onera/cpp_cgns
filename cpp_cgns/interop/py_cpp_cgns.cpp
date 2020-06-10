#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "cpp_cgns/interop/pycgns_converter.hpp"
#include "cpp_cgns/node_manip.hpp"

static PyObject*
cpp_cgns_ownership_test(PyObject* self, PyObject* args) {
  PyObject* base_pytree;
  if (!PyArg_ParseTuple(args, "O", &base_pytree)) return NULL;
  
  cgns::tree base = cgns::view_as_cpptree(base_pytree);

  cgns::cgns_allocator alloc; // allocates and owns memory

  //pytree_with_transfered_ownership_inplace(base,alloc,base_pytree);

  Py_INCREF(Py_None); return Py_None;
}

static PyMethodDef cpp_cgns_methods[] = {
  {"ownership_test",  cpp_cgns_ownership_test, METH_VARARGS, "ownership test"},
  {NULL, NULL, 0, NULL} /* Sentinel */
};

static PyModuleDef cpp_cgnsmodule = {
  PyModuleDef_HEAD_INIT,
  "cpp_cgns", /* name of module */
  NULL, /* module documentation, may be NULL */
  -1, /* size of per-interpreter state of the module,
         or -1 if the module keeps state in global variables. */
  cpp_cgns_methods
};

PyMODINIT_FUNC
PyInit_cpp_cgns(void) {
  return PyModule_Create(&cpp_cgnsmodule);
}
