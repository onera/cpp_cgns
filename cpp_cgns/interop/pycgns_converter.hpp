#pragma once


#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "cpp_cgns/cgns.hpp"


namespace cgns {

// Fwd decl
class cgns_allocator;


PyObject* view_as_pytree(tree& t);
tree view_as_cpptree(PyObject* pytree);

PyObject* pytree_with_transfered_ownership(tree& t, cgns_allocator& alloc);
//void pytree_with_transfered_ownership_inplace(tree& t, cgns_allocator& alloc, PyObject* pytree);
void add_new_nodes_and_ownership(tree& t, cgns_allocator& alloc, PyObject* pytree);
void update_and_transfer_ownership(tree& t, cgns_allocator& alloc, PyObject* pytree);
void update_and_transfer_ownership2(tree& t, cgns_allocator& alloc, PyObject* pytree);


} // cgns
