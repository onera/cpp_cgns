#include "cpp_cgns/interop/node_value_conversion.hpp"


#include "cpp_cgns/exception.hpp"
#include "std_e/buffer/buffer_span.hpp"


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
    throw cgns_exception("In python/CGNS, numpy array must be contiguous and fortran-ordered");
  }

  auto type = np_arr.dtype();
  auto data_type = numpy_type_to_cgns_type(type);

  auto n_dim = np_arr.ndim();

  auto dims_ptr = np_arr.shape();
  std::vector<I8> dims(n_dim);
  std::copy_n(dims_ptr,n_dim,begin(dims));

  void* data = np_arr.mutable_data();
  return {data_type,dims,std_e::buffer_span(data)};
}
auto
to_np_array(node_value& n, py::handle capsule) -> py::array {
  auto np_type = cgns_type_to_numpy_type(n.data_type);
  auto dt = py::dtype(np_type);
  auto strides = py::detail::f_strides(n.dims, dt.itemsize());
  return py::array(dt,n.dims,strides,data(n),capsule);
}
// numpy array <-> node_value }


} // cgns
