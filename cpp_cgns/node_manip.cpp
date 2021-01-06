#include "cpp_cgns/node_manip.hpp"


#include "std_e/utils/string.hpp"
#include "std_e/multi_array/utils.hpp"
#include "std_e/base/not_implemented_exception.hpp"
#include <algorithm>


namespace cgns {


/// to_string {
auto dims_to_string(const std::vector<I8>& dims) -> std::string {
  std::vector<std::string> dims_as_string(dims.size());
  std::transform(
    begin(dims),end(dims),
    begin(dims_as_string),
    [](I8 dim){ return std::to_string(dim); }
  );
  return std_e::join(dims_as_string,"x");
}

auto to_complete_string(const node_value& x) -> std::string {
  if (x.data_type=="I4") {
    return to_string(view_as_md_array<I4,dyn_rank>(x));
  } else if (x.data_type=="I8") {
    return to_string(view_as_md_array<I8,dyn_rank>(x));
  } else if (x.data_type=="R4") {
    return to_string(view_as_md_array<R4,dyn_rank>(x));
  } else if (x.data_type=="R8") {
    return to_string(view_as_md_array<R8,dyn_rank>(x));
  } else {
    throw std_e::not_implemented_exception("to_complete_string only implemented for I4, I8, R4, R8");
  }
}

auto to_string(const node_value& x, int threshold) -> std::string {
  if (x.data_type=="MT") return "MT";
  if (x.data_type=="C1") return std::string((const char*)x.data,x.dims[0]);
  if (std_e::cartesian_product_size(x.dims)<=threshold) return to_complete_string(x);
  else return "{"+x.data_type+":"+dims_to_string(x.dims)+"}";
}

auto to_string_impl(const tree& t, const std::string& indent, int threshold) -> std::string {
  static const std::string unit_indent = "  ";
  std::string s = indent + t.name + ", " + to_string(t.value,threshold) + ", " + t.label + "\n";
  for (const auto& c : t.children) {
    s += to_string_impl(c,indent+unit_indent,threshold);
  }
  return s;
}

auto to_string(const tree& t, int threshold) -> std::string {
  return to_string_impl(t,"",threshold);
}
/// to_string }


} // cgns
