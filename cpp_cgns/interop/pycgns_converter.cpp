#include "cpp_cgns/interop/pycgns_converter.hpp"

#include "std_e/future/contract.hpp"
#include "cpp_cgns/exception.hpp"
#include "numpy_config.hpp"
#include <algorithm>
#include "cpp_cgns/allocator.hpp"
//#include <pybind11/numpy.h>
#include "std_e/log.hpp" // TODO


namespace cgns {

// std::string <-> python string {
std::string to_std_string(PyObject* py_str) {
  #if PY_VERSION_HEX >= 0x03000000
    if (PyBytes_Check(py_str)) {
      return std::string(PyBytes_AsString(py_str));
    } else {
      STD_E_ASSERT(PyUnicode_Check(py_str));
      return PyBytes_AsString(PyUnicode_AsUTF8String(py_str));
    }
  #else
    return std::string(PyString_AsString(py_str));
  #endif
}

PyObject* to_python_string(const std::string& s) {
  #if PY_VERSION_HEX >= 0x03000000
    return PyUnicode_FromString(s.c_str());
  #else
    return PyString_FromString(s.c_str());
  #endif
}
// std::string <-> python string }


// typenum <-> string representation {
struct typenum_desc {
  int typenum;
  std::string desc;
};
const std::vector<typenum_desc> typenums_descs = {
  {NPY_BOOL         , "NPY_BOOL"},
  {NPY_BYTE         , "NPY_BYTE NPY_INT8"},
  {NPY_SHORT        , "NPY_SHORT NPY_INT16"},
  {NPY_INT          , "NPY_INT NPY_INT32"},
  {NPY_LONG         , "NPY_LONG"},
  {NPY_LONGLONG     , "NPY_LONGLONG NPY_INT64"},
  {NPY_UBYTE        , "NPY_UBYTE NPY_UINT8"},
  {NPY_USHORT       , "NPY_USHORT NPY_UINT16"},
  {NPY_UINT         , "NPY_UINT NPY_UINT32"},
  {NPY_ULONG        , "NPY_ULONG"},
  {NPY_ULONGLONG    , "NPY_ULONGLONG NPY_UINT64"},
  {NPY_HALF         , "NPY_HALF NPY_FLOAT16"},
  {NPY_FLOAT        , "NPY_FLOAT NPY_FLOAT32"},
  {NPY_DOUBLE       , "NPY_DOUBLE NPY_FLOAT64"},
  {NPY_LONGDOUBLE   , "NPY_LONGDOUBLE"},
  {NPY_CFLOAT       , "NPY_CFLOAT NPY_COMPLEX64"},
  {NPY_CDOUBLE      , "NPY_CDOUBLE NPY_COMPLEX128"},
  {NPY_CLONGDOUBLE  , "NPY_CLONGDOUBLE"},
  {NPY_DATETIME     , "NPY_DATETIME"},
  {NPY_TIMEDELTA    , "NPY_TIMEDELTA"},
  {NPY_STRING       , "NPY_STRING"},
  {NPY_UNICODE      , "NPY_UNICODE"},
  {NPY_OBJECT       , "NPY_OBJECT"},
  {NPY_VOID         , "NPY_VOID"},
  {NPY_INTP         , "NPY_INTP"},
  {NPY_UINTP        , "NPY_UINTP"},
//  {NPY_MASK         , "NPY_MASK"},
  {NPY_DEFAULT_TYPE , "NPY_DEFAULT_TYPE"},
  {NPY_NTYPES       , "NPY_NTYPES"},
  {NPY_NOTYPE       , "NPY_NOTYPE"},
  {NPY_USERDEF      , "NPY_USERDEF"},
};
std::string typenum_to_string(int typenum) {
  auto matching_typenum = [typenum](auto x){ return x.typenum==typenum; };
  auto pos = std::find_if(begin(typenums_descs),end(typenums_descs),matching_typenum);
  if (pos==end(typenums_descs)) {
    throw cgns_exception("Unknown typenum representation\""+std::to_string(typenum)+"\"");
  } else {
    return pos->desc;
  }
}
// typenum <-> string representation }


// data_type <-> typenum {
struct data_type_typenum {
  std::string data_type;
  int typenum;
};

const std::vector<data_type_typenum> data_types_typenums = {
  {"MT" , NPY_NOTYPE },
  {"C1" , NPY_STRING },
  //{"C1" , NPY_BYTE   }, // C1 -> NPY_BYTE (we would like C1 -> NPY_STRING, but then "ValueError: data type must provide an itemsize")
  //{"C1" , NPY_STRING }, // NPY_STRING -> C1 (this is what we want)
  {"I4" , NPY_INT32  },
  {"I8" , NPY_INT64  },
  {"R4" , NPY_FLOAT32},
  {"R8" , NPY_FLOAT64},
};


int data_type_to_typenum(const std::string& data_type) {
  auto matching_data_type = [&data_type](auto x){ return x.data_type==data_type; };
  auto pos = std::find_if(begin(data_types_typenums),end(data_types_typenums),matching_data_type);
  if (pos==end(data_types_typenums)) {
    throw cgns_exception("Unknown data type \""+data_type+"\"");
  } else {
    return pos->typenum;
  }
}
std::string typenum_to_data_type(int typenum) {
  auto matching_typenum = [typenum](auto x){ return x.typenum==typenum; };
  auto pos = std::find_if(begin(data_types_typenums),end(data_types_typenums),matching_typenum);
  if (pos==end(data_types_typenums)) {
    throw cgns_exception("Unknown typenum \""+typenum_to_string(typenum)+"\"");
  } else {
    return pos->data_type;
  }
}
// data_type <-> typenum }


// node_value <-> numpy_array {
PyObject* view_as_numpy_array(node_value& n) {
  int typenum = data_type_to_typenum(n.data_type);
  PyArray_Descr* descr = PyArray_DescrFromType(typenum);
  if (typenum==NPY_STRING) {
    PyObject* descr_str = to_python_string("|S1");
    PyArray_DescrConverter( descr_str, &descr );
  }
  return PyArray_NewFromDescr(&PyArray_Type, descr, n.dims.size(), n.dims.data(), NULL, n.data, NPY_ARRAY_F_CONTIGUOUS, NULL);
}

node_value view_as_node(PyArrayObject* numpy_array) {
  if (!PyArray_IS_F_CONTIGUOUS(numpy_array)) {
    throw cgns_exception("In python/CGNS, numpy array must be contiguous and fortran-ordered");
  }

  int typenum = PyArray_TYPE(numpy_array);
  std::string data_type = typenum_to_data_type(typenum);

  I4 nb_dims = PyArray_NDIM(numpy_array);

  npy_intp* dims_ptr = PyArray_DIMS(numpy_array);
  std::vector<I8> dims(nb_dims);
  std::copy_n(dims_ptr,nb_dims,begin(dims));

  void* data = PyArray_DATA(numpy_array);
  return {data_type,dims,data};
}
// node_value <-> numpy_array }


// cgns::tree attributes <-> pytree attributes {
std::string get_py_name(PyObject* pytree) {
  return to_std_string(PyList_GetItem(pytree,0));
}
void set_py_name(PyObject* pytree, const std::string& name) {
  PyList_SetItem(pytree,0,to_python_string(name));
}
void set_py_name(py::list pytree, const std::string& name) {
  pytree[0] = to_python_string(name);
}

std::string get_py_label(PyObject* pytree) {
  return to_std_string(PyList_GetItem(pytree,3));
}
void set_py_label(PyObject* pytree, const std::string& label) {
  PyObject* py_label = to_python_string(label);
  PyList_SetItem(pytree,3,py_label);
}
void set_py_label(py::list pytree, const std::string& label) {
  PyObject* py_label = to_python_string(label);
  pytree[3] = py_label;
}

node_value get_py_value(PyObject* pytree) {
  PyObject* py_value = PyList_GetItem(pytree,1);
  if (py_value==Py_None) {
    return MT;
  } else {
    PyArrayObject* numpy_array = (PyArrayObject*)py_value;
    return view_as_node(numpy_array);
  }
}
void set_py_value(PyObject* pytree, node_value& value) { // NOTE: non const ref because shared data
  PyObject* py_value;
  if (value.data_type=="MT") {
    Py_INCREF(Py_None); // TODO not sure it is needed
    py_value = Py_None;
  } else {
    py_value = view_as_numpy_array(value);
  }
  PyList_SetItem(pytree,1,py_value);
}
void set_py_value(py::list pytree, node_value& value) { // NOTE: non const ref because shared data
  PyObject* py_value;
  if (value.data_type=="MT") {
    Py_INCREF(Py_None); // TODO not sure it is needed
    py_value = Py_None;
  } else {
    py_value = view_as_numpy_array(value);
  }
  pytree[1] = py_value;
}
// cgns::tree attributes <-> pytree attributes }




py::list py_tree() {
  return py::list(4);
}

auto name(py::list t) {
  return t[0];
}
py::str name3(py::list t) {
  return t[0];
}
void set_name4(py::list t, const std::string& cs) {
  //py::str s = t[0];
  //s = cs;
  t[0] = cs;
}
auto value(py::list t) {
  return t[1];
}
auto children(py::list t) {
  return t[2];
}
auto label(py::list t) {
  return t[3];
}
template<class T>
std::string to_string3(T& x) {
  return py::str(x);
}

// tree <-> pytree {
py::object view_as_pytree(tree& t) {
  _import_array(); // IMPORTANT needed for Numpy C API initialization (else: segfault)
  auto pytree = py_tree();

  set_name4 (pytree, name (t));
  label(pytree) = label(t);
  set_py_value(pytree,value(t));

  py::list py_children(t.children.size());
  for (size_t i=0; i<t.children.size(); ++i) {
    py::object py_child = view_as_pytree(t.children[i]);
    py_children[i] = py_child;
  }
  children(pytree) = py_children;

  return pytree;
}


tree view_as_cpptree(py::list pytree) {
  _import_array(); // IMPORTANT needed for Numpy C API initialization (else: segfault) // TODO check

  //auto xx = name(pytree);
  //std::string name2 = to_string3(xx); //get_py_name(pytree.ptr());
  std::string name2 = name3(pytree);
  std::string label = get_py_label(pytree.ptr());
  node_value value = get_py_value(pytree.ptr());

  py::list py_children = children(pytree);
  int nb_children = py_children.size();
  std::vector<tree> children(nb_children);
  for (int i=0; i<nb_children; ++i) {
    py::list py_child = py_children[i];
    children[i] = view_as_cpptree(py_child);
  }

  return {name2,value,children,label};
}
// tree <-> pytree }


// tree -> pytree with ownership transfer {
// SEE https://stackoverflow.com/a/52737023/1583122
void capsule_cleanup_by_free(PyObject* capsule) {
  void* memory = PyCapsule_GetPointer(capsule, NULL);
  free(memory); // because cgns_allocator uses malloc/free
}
PyObject* make_owning_numpy_array(node_value& n) {
  PyObject* np_arr = view_as_numpy_array(n);
  PyObject* capsule = PyCapsule_New(n.data, NULL, capsule_cleanup_by_free);
  PyArray_SetBaseObject((PyArrayObject*) np_arr, capsule);
  return np_arr;
}

PyObject* pytree_with_transfered_ownership(tree& t, cgns_allocator& alloc) {
  // creates a pytree from t
  // if t memory was allocated with alloc,
  //   then we release the ownship from alloc
  //   and create a numpy array owning it (PyArray_SimpleNewFromData && cleanup capsule)
  // if not, we only create a non-owning numpy array (PyArray_SimpleNewFromData)

  _import_array(); // IMPORTANT needed for Numpy C API initialization (else: segfault)
  PyObject* pytree = PyList_New(4);

  set_py_name(pytree,t.name);
  set_py_label(pytree,t.label);

  if (alloc.release_if_owner(t.value.data)) {
    PyObject* py_value = make_owning_numpy_array(t.value);
    PyList_SetItem(pytree,1,py_value);
  } else {
    set_py_value(pytree,t.value);
  }

  PyObject* py_children = PyList_New(t.children.size());
  for (size_t i=0; i<t.children.size(); ++i) {
    PyObject* py_child = pytree_with_transfered_ownership(t.children[i],alloc);
    PyList_SetItem(py_children,i,py_child);
  }
  PyList_SetItem(pytree,2,py_children);

  return pytree;
}
//void pytree_with_transfered_ownership_inplace(tree& t, cgns_allocator& alloc, PyObject* pytree) {
//  // creates a pytree from t
//  // if t memory was allocated with alloc,
//  //   then we release the ownship from alloc
//  //   and create a numpy array owning it (PyArray_SimpleNewFromData && cleanup capsule)
//  // if not, we only create a non-owning numpy array (PyArray_SimpleNewFromData)
//
//  _import_array(); // IMPORTANT needed for Numpy C API initialization (else: segfault)
//
//  set_py_name(pytree,t.name);
//  set_py_label(pytree,t.label);
//
//  if (alloc.release_if_owner(t.value.data)) {
//    PyObject* py_value = make_owning_numpy_array(t.value);
//    PyList_SetItem(pytree,1,py_value);
//  } else {
//    set_py_value(pytree,t.value);
//  }
//
//  PyObject* py_children = PyList_New(t.children.size());
//  for (size_t i=0; i<t.children.size(); ++i) {
//    PyObject* py_child = pytree_with_transfered_ownership(t.children[i],alloc);
//    PyList_SetItem(py_children,i,py_child);
//  }
//  PyList_SetItem(pytree,2,py_children);
//}

bool same_node_data(const node_value& x, const node_value& y) {
  return
      x.data_type==y.data_type
   && x.dims==y.dims
   && x.data==y.data;
}
void add_new_nodes_and_ownership(tree& t, cgns_allocator& alloc, PyObject* pytree) {
  // precondition: t\pytree == empty set

  _import_array(); // IMPORTANT needed for Numpy C API initialization (else: segfault)

  STD_E_ASSERT(name(t)==get_py_name(pytree));
  STD_E_ASSERT(label(t)==get_py_label(pytree));
  STD_E_ASSERT(same_node_data(t.value,get_py_value(pytree)));

  PyObject* py_children = PyList_GetItem(pytree,2);
  int nb_py_children = PyList_Size(py_children);
  int nb_children = t.children.size();
  STD_E_ASSERT(nb_py_children<=nb_children);
  for (int i=0; i<nb_py_children; ++i) {
    PyObject* py_child = PyList_GetItem(py_children,i);
    add_new_nodes_and_ownership(t.children[i],alloc,py_child);
  }
  for (int i=nb_py_children; i<nb_children; ++i) {
    PyObject* py_child = pytree_with_transfered_ownership(t.children[i],alloc);
    PyList_Append(py_children,py_child);
    Py_DECREF(py_child); // TODO check
  }
}

void update_and_transfer_ownership(tree& t, cgns_allocator& alloc, PyObject* pytree) {
  // preconditions:
  //   - nodes similar in t and pytree have the same order
  //     (in other words, t nodes were never reordered)
  //   - nodes are identified by name (no renaming in t)

  _import_array(); // IMPORTANT needed for Numpy C API initialization (else: segfault)

  STD_E_ASSERT(name(t)==get_py_name(pytree));
  STD_E_ASSERT(label(t)==get_py_label(pytree));
  STD_E_ASSERT(same_node_data(t.value,get_py_value(pytree)));

  PyObject* py_children = PyList_GetItem(pytree,2);
  int nb_children = t.children.size();
  int i = 0;
  int i_py = 0;
  while (i_py < PyList_Size(py_children)) {
    PyObject* py_child = PyList_GetItem(py_children,i_py);
    if (i<nb_children && t.children[i].name==get_py_name(py_child)) {
      update_and_transfer_ownership(t.children[i],alloc,py_child);
      ++i;
      ++i_py;
    } else { // since t and pytree have the same order
             // it means an item has been removed in t
             // and this should be propagated to pytree
      PyList_SetSlice(py_children, i_py, i_py+1, NULL); // rm item at i_py
      Py_DECREF(py_child); // TODO check
    }
  }
  for (; i<nb_children; ++i) {
    PyObject* py_child = pytree_with_transfered_ownership(t.children[i],alloc);
    PyList_Append(py_children,py_child);
    Py_DECREF(py_child); // TODO check
  }
}

// TODO DEL the other (but test that it does not break anything!)
void update_and_transfer_ownership2(tree& t, cgns_allocator& alloc, PyObject* pytree) {
  // preconditions:
  //   - nodes similar in t and pytree have the same order
  //     (in other words, t nodes were never reordered)
  //   - nodes are identified by name (no renaming in t)

  _import_array(); // IMPORTANT needed for Numpy C API initialization (else: segfault)

  STD_E_ASSERT(name(t)==get_py_name(pytree));
  STD_E_ASSERT(label(t)==get_py_label(pytree));

  if (!same_node_data(t.value,get_py_value(pytree))) {
    // TODO factor with pytree_with_transfered_ownership(t.children[i],alloc,py_child);
    if (alloc.release_if_owner(t.value.data)) {
      PyObject* py_value = make_owning_numpy_array(t.value);
      PyList_SetItem(pytree,1,py_value);
    } else {
      set_py_value(pytree,t.value);
    }
  }

  PyObject* py_children = PyList_GetItem(pytree,2);
  int nb_children = t.children.size();
  int i = 0;
  int i_py = 0;
  while (i_py < PyList_Size(py_children)) {
    PyObject* py_child = PyList_GetItem(py_children,i_py);
    if (i<nb_children && t.children[i].name==get_py_name(py_child)) {
      update_and_transfer_ownership2(t.children[i],alloc,py_child);
      ++i;
      ++i_py;
    } else { // since t and pytree have the same order
             // it means an item has been removed in t
             // and this should be propagated to pytree
      PyList_SetSlice(py_children, i_py, i_py+1, NULL); // rm item at i_py
      Py_DECREF(py_child); // TODO check
    }
  }
  for (; i<nb_children; ++i) {
    PyObject* py_child = pytree_with_transfered_ownership(t.children[i],alloc);
    PyList_Append(py_children,py_child);
    Py_DECREF(py_child); // TODO check
  }
}
// tree -> pytree with ownership transfer }

} // cgns
