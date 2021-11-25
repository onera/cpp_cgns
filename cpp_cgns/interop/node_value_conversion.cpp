#include "cpp_cgns/interop/node_value_conversion.hpp"


#include "cpp_cgns/base/exception.hpp"
#include "std_e/multi_index/cartesian_product_size.hpp"


namespace cgns {


// data_type <-> numpy type {
struct cgns_numpy_type {
  std::string cgns_type;
  std::string np_type;
};
const std::vector<cgns_numpy_type> cgns_numpy_types = {
  {"C1" , "|S1"},
  {"I4" , "int32"},
  {"I8" , "int64"},
  {"R4" , "float32"},
  {"R8" , "float64"},
};

auto
numpy_type_to_cgns_type(py::dtype type) -> std::string {
  auto np_type = std::string(py::str(type));
  auto matching_numpy_type = [np_type](auto x){ return x.np_type==np_type; };
  auto pos = std::find_if(begin(cgns_numpy_types),end(cgns_numpy_types),matching_numpy_type);
  if (pos==end(cgns_numpy_types)) {
    throw cgns_exception("Unknown numpy type \""+np_type+"\"");
  } else {
    return pos->cgns_type;
  }
}
auto
cgns_type_to_numpy_type(const std::string& cgns_type) -> std::string {
  auto matching_data_type = [&cgns_type](auto x){ return x.cgns_type==cgns_type; };
  auto pos = std::find_if(begin(cgns_numpy_types),end(cgns_numpy_types),matching_data_type);
  if (pos==end(cgns_numpy_types)) {
    throw cgns_exception("Unknown cgns data type \""+cgns_type+"\"");
  } else {
    return pos->np_type;
  }
}
// data_type <-> numpy type }


// numpy array <-> node_value {
auto
view_as_node_value(py::array np_arr) -> node_value {
  if (!(np_arr.flags() & py::array::f_style)) {
    throw cgns_exception("In Python/CGNS, numpy arrays must be contiguous and fortran-ordered");
  }

  auto type = np_arr.dtype();
  auto data_type = numpy_type_to_cgns_type(type);

  auto n_dim = np_arr.ndim();

  auto dims_ptr = np_arr.shape();
  std::vector<I8> dims(n_dim);
  std::copy_n(dims_ptr,n_dim,begin(dims));

  void* data = np_arr.mutable_data();
  return make_non_owning_node_value(data_type,data,std::move(dims));
}
auto
copy_to_node_value(py::array np_arr) -> node_value {
  if (!(np_arr.flags() & py::array::f_style)) {
    throw cgns_exception("In Python/CGNS, numpy arrays must be contiguous and fortran-ordered");
  }

  auto type = np_arr.dtype();
  auto data_type = numpy_type_to_cgns_type(type);

  auto n_dim = np_arr.ndim();

  auto dims_ptr = np_arr.shape();
  std::vector<I8> dims(n_dim);
  std::copy_n(dims_ptr,n_dim,begin(dims));

  const void* data = np_arr.data();
  return make_node_value(data_type,data,std::move(dims));
}

auto
to_np_array(node_value& n, py::handle capsule) -> py::array {
  auto np_type = cgns_type_to_numpy_type(n.data_type());
  auto dt = py::dtype(np_type);
  auto strides = py::detail::f_strides(n.extent(), dt.itemsize());
  return py::array(dt,n.extent(),strides,n.data(),capsule);
}


constexpr auto release_memory_fn = []<class T>(std_e::polymorphic_array<T>& arr) -> std::pair<void*,py::capsule> {
  using polymorphic_base = typename std_e::polymorphic_array<T>::polymorphic_base;

  polymorphic_base* arr_base = arr.release(); // Release the state "arr_base" of "arr"
                                              // Now "arr" is not reponsible for its state "arr_base" anymore
                                              // Hence "arr" can be destroyed while the memory is still alive...
  void* data = arr_base->data(); // ... which is good because we want to access this memory from Python...
  return {data,py::capsule(arr_base, arr.cleanup_fn())}; // ... but then we also need to give Python a capsule
                                                         // to delete the memory when it doesn't need it anymore
};
auto
release_memory(node_value_array& nv_arr) -> std::pair<void*,py::capsule> {
  auto& var_arr = nv_arr.underlying_variant();
  return std::visit(release_memory_fn, var_arr);
}
auto
to_owning_np_array(node_value&& n) -> py::array {
  auto np_type = cgns_type_to_numpy_type(n.data_type());
  auto dt = py::dtype(np_type);
  auto strides = py::detail::f_strides(n.extent(), dt.itemsize());

  node_value_array& nv_arr = n.underlying_range();
  auto [data,capsule] = release_memory(nv_arr);
  return py::array(dt,n.extent(),strides,data,capsule);
}
auto
to_empty_np_array(const std::string& data_type) -> py::array {
  auto np_type = cgns_type_to_numpy_type(data_type);
  auto dt = py::dtype(np_type);
  std::vector<I8> no_dims = {};
  auto strides = py::detail::f_strides(no_dims, dt.itemsize());
  return py::array(dt,no_dims,strides,nullptr);
}
// numpy array <-> node_value }

// python string -> node_value {
auto
copy_py_string_to_node_value(py::object str) -> node_value {
  if (!PyUnicode_Check(str.ptr())) {
    throw cgns_exception("node_value is a string, but it is not unicode");
  }

  const char* buffer = nullptr;
  ssize_t length = 0;
  buffer = PyUnicode_AsUTF8AndSize(str.ptr(),&length);

  return node_value(std::vector<char>(buffer,buffer+length));
}
// python string -> node_value }


} // cgns
