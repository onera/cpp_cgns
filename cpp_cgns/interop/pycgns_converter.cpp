#include "cpp_cgns/interop/pycgns_converter.hpp"

#include "std_e/future/contract.hpp"
#include "cpp_cgns/exception.hpp"
//#include "numpy_config.hpp"
#include <algorithm>
#include "cpp_cgns/allocator.hpp"
#include "cpp_cgns/node_manip.hpp"
#include <pybind11/numpy.h>
#include "std_e/log.hpp" // TODO


namespace cgns {

//// std::string <-> python string {
//std::string to_std_string(PyObject* py_str) {
//  #if PY_VERSION_HEX >= 0x03000000
//    if (PyBytes_Check(py_str)) {
//      return std::string(PyBytes_AsString(py_str));
//    } else {
//      STD_E_ASSERT(PyUnicode_Check(py_str));
//      return PyBytes_AsString(PyUnicode_AsUTF8String(py_str));
//    }
//  #else
//    return std::string(PyString_AsString(py_str));
//  #endif
//}
//
//PyObject* to_python_string(const std::string& s) {
//  #if PY_VERSION_HEX >= 0x03000000
//    return PyUnicode_FromString(s.c_str());
//  #else
//    return PyString_FromString(s.c_str());
//  #endif
//}
//// std::string <-> python string }


//// typenum <-> string representation {
//struct typenum_desc {
//  int typenum;
//  std::string desc;
//};
//const std::vector<typenum_desc> typenums_descs = {
//  {NPY_BOOL         , "NPY_BOOL"},
//  {NPY_BYTE         , "NPY_BYTE NPY_INT8"},
//  {NPY_SHORT        , "NPY_SHORT NPY_INT16"},
//  {NPY_INT          , "NPY_INT NPY_INT32"},
//  {NPY_LONG         , "NPY_LONG"},
//  {NPY_LONGLONG     , "NPY_LONGLONG NPY_INT64"},
//  {NPY_UBYTE        , "NPY_UBYTE NPY_UINT8"},
//  {NPY_USHORT       , "NPY_USHORT NPY_UINT16"},
//  {NPY_UINT         , "NPY_UINT NPY_UINT32"},
//  {NPY_ULONG        , "NPY_ULONG"},
//  {NPY_ULONGLONG    , "NPY_ULONGLONG NPY_UINT64"},
//  {NPY_HALF         , "NPY_HALF NPY_FLOAT16"},
//  {NPY_FLOAT        , "NPY_FLOAT NPY_FLOAT32"},
//  {NPY_DOUBLE       , "NPY_DOUBLE NPY_FLOAT64"},
//  {NPY_LONGDOUBLE   , "NPY_LONGDOUBLE"},
//  {NPY_CFLOAT       , "NPY_CFLOAT NPY_COMPLEX64"},
//  {NPY_CDOUBLE      , "NPY_CDOUBLE NPY_COMPLEX128"},
//  {NPY_CLONGDOUBLE  , "NPY_CLONGDOUBLE"},
//  {NPY_DATETIME     , "NPY_DATETIME"},
//  {NPY_TIMEDELTA    , "NPY_TIMEDELTA"},
//  {NPY_STRING       , "NPY_STRING"},
//  {NPY_UNICODE      , "NPY_UNICODE"},
//  {NPY_OBJECT       , "NPY_OBJECT"},
//  {NPY_VOID         , "NPY_VOID"},
//  {NPY_INTP         , "NPY_INTP"},
//  {NPY_UINTP        , "NPY_UINTP"},
////  {NPY_MASK         , "NPY_MASK"},
//  {NPY_DEFAULT_TYPE , "NPY_DEFAULT_TYPE"},
//  {NPY_NTYPES       , "NPY_NTYPES"},
//  {NPY_NOTYPE       , "NPY_NOTYPE"},
//  {NPY_USERDEF      , "NPY_USERDEF"},
//};
//std::string typenum_to_string(int typenum) {
//  auto matching_typenum = [typenum](auto x){ return x.typenum==typenum; };
//  auto pos = std::find_if(begin(typenums_descs),end(typenums_descs),matching_typenum);
//  if (pos==end(typenums_descs)) {
//    throw cgns_exception("Unknown typenum representation\""+std::to_string(typenum)+"\"");
//  } else {
//    return pos->desc;
//  }
//}
//// typenum <-> string representation }


//// data_type <-> typenum {
//struct data_type_typenum {
//  std::string data_type;
//  int typenum;
//};
//
//const std::vector<data_type_typenum> data_types_typenums = {
//  {"MT" , NPY_NOTYPE },
//  {"C1" , NPY_BYTE},
//  //{"C1" , NPY_BYTE   }, // C1 -> NPY_BYTE (we would like C1 -> NPY_STRING, but then "ValueError: data type must provide an itemsize")
//  //{"C1" , NPY_STRING }, // NPY_STRING -> C1 (this is what we want)
//  {"I4" , NPY_INT32  },
//  {"I8" , NPY_INT64  },
//  {"R4" , NPY_FLOAT32},
//  {"R8" , NPY_FLOAT64},
//};
//
//
//int data_type_to_typenum(const std::string& data_type) {
//  auto matching_data_type = [&data_type](auto x){ return x.data_type==data_type; };
//  auto pos = std::find_if(begin(data_types_typenums),end(data_types_typenums),matching_data_type);
//  if (pos==end(data_types_typenums)) {
//    throw cgns_exception("Unknown data type \""+data_type+"\"");
//  } else {
//    return pos->typenum;
//  }
//}
//std::string typenum_to_data_type(int typenum) {
//  auto matching_typenum = [typenum](auto x){ return x.typenum==typenum; };
//  auto pos = std::find_if(begin(data_types_typenums),end(data_types_typenums),matching_typenum);
//  if (pos==end(data_types_typenums)) {
//    throw cgns_exception("Unknown typenum \""+typenum_to_string(typenum)+"\"");
//  } else {
//    return pos->data_type;
//  }
//}
//// data_type <-> typenum }


//// node_value <-> numpy_array {
//PyObject* view_as_numpy_array(node_value& n) {
//  int typenum = data_type_to_typenum(n.data_type);
//  PyArray_Descr* descr = PyArray_DescrFromType(typenum);
//  if (typenum==NPY_STRING) {
//    PyObject* descr_str = to_python_string("|S1");
//    PyArray_DescrConverter( descr_str, &descr );
//  }
//  return PyArray_NewFromDescr(&PyArray_Type, descr, n.dims.size(), n.dims.data(), NULL, n.data, NPY_ARRAY_F_CONTIGUOUS, NULL);
//}
//
//node_value view_as_node(PyArrayObject* numpy_array) {
//  if (!PyArray_IS_F_CONTIGUOUS(numpy_array)) {
//    throw cgns_exception("In python/CGNS, numpy array must be contiguous and fortran-ordered");
//  }
//
//  int typenum = PyArray_TYPE(numpy_array);
//  std::string data_type = typenum_to_data_type(typenum);
//
//  I4 nb_dims = PyArray_NDIM(numpy_array);
//
//  npy_intp* dims_ptr = PyArray_DIMS(numpy_array);
//  std::vector<I8> dims(nb_dims);
//  std::copy_n(dims_ptr,nb_dims,begin(dims));
//
//  void* data = PyArray_DATA(numpy_array);
//  return {data_type,dims,data};
//}
//// node_value <-> numpy_array }


//// cgns::tree attributes <-> pytree attributes {
//std::string get_py_name(PyObject* pytree) {
//  return to_std_string(PyList_GetItem(pytree,0));
//}
//void set_py_name(PyObject* pytree, const std::string& name) {
//  PyList_SetItem(pytree,0,to_python_string(name));
//}
//void set_py_name(py::list pytree, const std::string& name) {
//  pytree[0] = to_python_string(name);
//}
//
//std::string get_py_label(PyObject* pytree) {
//  return to_std_string(PyList_GetItem(pytree,3));
//}
//void set_py_label(PyObject* pytree, const std::string& label) {
//  PyObject* py_label = to_python_string(label);
//  PyList_SetItem(pytree,3,py_label);
//}
//void set_py_label(py::list pytree, const std::string& label) {
//  PyObject* py_label = to_python_string(label);
//  pytree[3] = py_label;
//}
//
//node_value get_py_value(PyObject* pytree) {
//  PyObject* py_value = PyList_GetItem(pytree,1);
//  if (py_value==Py_None) {
//    return MT;
//  } else {
//    PyArrayObject* numpy_array = (PyArrayObject*)py_value;
//    return view_as_node(numpy_array);
//  }
//}
//void set_py_value(PyObject* pytree, node_value& value) { // NOTE: non const ref because shared data
//  PyObject* py_value;
//  if (value.data_type=="MT") {
//    Py_INCREF(Py_None); // TODO not sure it is needed
//    py_value = Py_None;
//  } else {
//    py_value = view_as_numpy_array(value);
//  }
//  PyList_SetItem(pytree,1,py_value);
//}
//// cgns::tree attributes <-> pytree attributes }




py::list py_tree() {
  return py::list(4);
}

auto name(py::list t) {
  return t[0];
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
std::string to_string_from_py(const T& x) {
  return py::str(x);
}

// data_type <-> numpy type {
struct cgns_numpy_type {
  std::string cgns_type;
  std::string numpy_type;
};
const std::vector<cgns_numpy_type> cgns_numpy_types = {
  {"C1" , "int8"},
  {"I4" , "int32"},
  {"I8" , "int64"},
  {"R4" , "f4"},
  {"R8" , "f8"},
};
auto numpy_type_to_cgns_type(py::dtype type) -> std::string {
  auto numpy_type = std::string(py::str(type));
  auto matching_numpy_type = [numpy_type](auto x){ return x.numpy_type==numpy_type; };
  auto pos = std::find_if(begin(cgns_numpy_types),end(cgns_numpy_types),matching_numpy_type);
  if (pos==end(cgns_numpy_types)) {
    throw cgns_exception("Unknown numpy type \""+numpy_type+"\"");
  } else {
    return pos->cgns_type;
  }
}
auto cgns_type_to_numpy_type(const std::string& cgns_type) -> std::string {
  auto matching_data_type = [&cgns_type](auto x){ return x.cgns_type==cgns_type; };
  auto pos = std::find_if(begin(cgns_numpy_types),end(cgns_numpy_types),matching_data_type);
  if (pos==end(cgns_numpy_types)) {
    throw cgns_exception("Unknown cgns data type \""+cgns_type+"\"");
  } else {
    return pos->numpy_type;
  }
}
// data_type <-> numpy type }


node_value view_as_node2(py::array numpy_array) {
  if (!(numpy_array.flags() & py::array::f_style)) {
    throw cgns_exception("In python/CGNS, numpy array must be contiguous and fortran-ordered");
  }

  auto type = numpy_array.dtype();
  std::string data_type = numpy_type_to_cgns_type(type);

  ssize_t nb_dims = numpy_array.ndim();

  const ssize_t* dims_ptr = numpy_array.shape();
  std::vector<I8> dims(nb_dims);
  std::copy_n(dims_ptr,nb_dims,begin(dims));

  // TODO
  //ELOG(numpy_array.writeable());
  //void* data = numpy_array.mutable_data();
  void* data = (void*)numpy_array.data();
  return {data_type,dims,data};
}
node_value get_py_value2(py::list t) {
  auto val = value(t);
  if (val.is_none()) {
    return MT;
  } else {
    return view_as_node2(val);
  }
}


py::array view_as_numpy_array2(node_value& n, py::handle capsule = py::handle()) {
  auto numpy_type = cgns_type_to_numpy_type(n.data_type);
  auto dt = py::dtype(numpy_type);
  auto strides = py::detail::f_strides(n.dims, dt.itemsize());
  return py::array(dt,n.dims,strides,n.data,capsule);
}
void set_py_value2(py::list pytree, node_value& value) { // NOTE: non const ref because shared data
  py::object py_value;
  if (value.data_type=="MT") {
    py_value = py::none();
  } else {
    py_value = view_as_numpy_array2(value);
  }
  pytree[1] = py_value;
  //value(pytree) = py_value;
}

// tree <-> pytree {
py::object view_as_pytree(tree& t) {
  auto pytree = py_tree();

  name (pytree) = name (t);
  label(pytree) = label(t);
  set_py_value2(pytree,value(t));

  py::list py_children(t.children.size());
  for (size_t i=0; i<t.children.size(); ++i) {
    py::object py_child = view_as_pytree(t.children[i]);
    py_children[i] = py_child;
  }
  children(pytree) = py_children;

  return pytree;
}


tree view_as_cpptree(py::list pytree) {
  std::string name2  = to_string_from_py(name (pytree));
  std::string label2 = to_string_from_py(label(pytree));
  node_value value = get_py_value2(pytree);

  py::list py_children = children(pytree);
  int nb_children = py_children.size();
  std::vector<tree> children(nb_children);
  for (int i=0; i<nb_children; ++i) {
    py::list py_child = py_children[i];
    children[i] = view_as_cpptree(py_child);
  }

  return {name2,label2,value,children};
}
// tree <-> pytree }


// tree -> pytree with ownership transfer {
// SEE https://stackoverflow.com/a/52737023/1583122
void capsule_cleanup_by_free(PyObject* capsule) {
  void* memory = PyCapsule_GetPointer(capsule, NULL);
  free(memory); // because cgns_allocator uses malloc/free
}
py::array make_owning_numpy_array(node_value& n) {
  auto capsule = py::capsule(n.data, capsule_cleanup_by_free);
  return view_as_numpy_array2(n,capsule);
}
void set_py_value2_owner(py::list pytree, node_value& value) { // NOTE: non const ref because shared data
  py::object py_value;
  if (value.data_type=="MT") {
    py_value = py::none();
  } else {
    py_value = make_owning_numpy_array(value);
  }
  pytree[1] = py_value;
  //value(pytree) = py_value;
}

py::object pytree_with_transfered_ownership(tree& t, cgns_allocator& alloc) {
  // creates a pytree from t
  // if t memory was allocated with alloc,
  //   then we release the ownship from alloc
  //   and create a numpy array owning it (PyArray_SimpleNewFromData && cleanup capsule)
  // if not, we only create a non-owning numpy array (PyArray_SimpleNewFromData)

  auto pytree = py_tree();

  name (pytree) = name (t);
  label(pytree) = label(t);

  if (alloc.release_if_owner(t.value.data)) {
    set_py_value2_owner(pytree,value(t));
  } else {
    set_py_value2(pytree,value(t));
  }

  py::list py_children(t.children.size());
  for (size_t i=0; i<t.children.size(); ++i) {
    py::object py_child = pytree_with_transfered_ownership(t.children[i],alloc);
    py_children[i] = py_child;
  }
  children(pytree) = py_children;

  return pytree;
}


void update_and_transfer_ownership2(tree& t, cgns_allocator& alloc, py::list pytree) {
  // preconditions:
  //   - nodes similar in t and pytree have the same order
  //     (in other words, t nodes were never reordered)
  //   - nodes are identified by name (no renaming in t)
  STD_E_ASSERT(name (t)==name (pytree));
  STD_E_ASSERT(label(t)==label(pytree));

  if (!same_node_data(value(t),get_py_value2(pytree))) {
    // TODO factor with pytree_with_transfered_ownership(t.children[i],alloc,py_child);
    if (alloc.release_if_owner(t.value.data)) {
      set_py_value2_owner(pytree,value(t));
    } else {
      set_py_value2(pytree,value(t));
    }
  }

  py::list py_children = children(pytree);
  int nb_children = t.children.size();
  int i = 0;
  int i_py = 0;
  while (i_py < py_children.size()) {
    auto py_child = py_children[i_py];
    if (i<nb_children && name(t.children[i])==to_string_from_py(name(py_child))) {
      update_and_transfer_ownership2(t.children[i],alloc,py_child);
      ++i;
      ++i_py;
    } else { // since t and pytree have the same order
             // it means an item has been removed in t
             // and this should be propagated to pytree
      PyList_SetSlice(py_children.ptr(), i_py, i_py+1, NULL); // rm item at i_py TODO pybind API
    }
  }
  for (; i<nb_children; ++i) {
    py::object py_child = pytree_with_transfered_ownership(t.children[i],alloc);
    py_children.append(py_child);
  }
}
// tree -> pytree with ownership transfer }

} // cgns
