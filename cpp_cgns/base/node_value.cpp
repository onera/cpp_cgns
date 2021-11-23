#include "cpp_cgns/base/node_value.hpp"


#include "std_e/multi_array/utils.hpp"
#include "cpp_cgns/base/node_value_conversion.hpp"


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
  // TODO use visit
  if (x.data_type()=="I4") {
    return to_string(view_as_md_array<I4,dyn_rank>(x));
  } else if (x.data_type()=="I8") {
    return to_string(view_as_md_array<I8,dyn_rank>(x));
  } else if (x.data_type()=="R4") {
    return to_string(view_as_md_array<R4,dyn_rank>(x));
  } else if (x.data_type()=="R8") {
    return to_string(view_as_md_array<R8,dyn_rank>(x));
  } else {
    throw std_e::not_implemented_exception("to_complete_string only implemented for I4, I8, R4, R8");
  }
}

auto to_string(const node_value& x, int threshold) -> std::string {
  if (x.data_type()=="MT") return "MT";
  if (x.data_type()=="C1") return std::string((const char*)x.data(),x.extent(0));
  if (std_e::cartesian_product_size(x.extent())<=threshold) return to_complete_string(x);
  else return "{"+x.data_type()+":"+dims_to_string(x.extent())+"}";
}
// to_string }


// make_node_value {
// TODO facto
auto make_node_value(const std::string& data_type, const std::vector<I8>& dims, const void* data) -> node_value {
  // TODO use visit
  auto sz = std_e::cartesian_product_size(dims);
  if (data_type=="C1") {
    auto* ptr = static_cast<const C1*>(data);
    std::vector<C1> v(ptr,ptr+sz);
    return node_value(md_array<C1,dyn_rank>(std::move(v),md_array_shape<dyn_rank>(dims)));
  } else if (data_type=="I4") {
    auto* ptr = static_cast<const I4*>(data);
    std::vector<I4> v(ptr,ptr+sz);
    return node_value(md_array<I4,dyn_rank>(std::move(v),md_array_shape<dyn_rank>(dims)));
  } else if (data_type=="I8") {
    auto* ptr = static_cast<const I8*>(data);
    std::vector<I8> v(ptr,ptr+sz);
    return node_value(md_array<I8,dyn_rank>(std::move(v),md_array_shape<dyn_rank>(dims)));
  } else if (data_type=="R4") {
    auto* ptr = static_cast<const R4*>(data);
    std::vector<R4> v(ptr,ptr+sz);
    return node_value(md_array<R4,dyn_rank>(std::move(v),md_array_shape<dyn_rank>(dims)));
  } else if (data_type=="R8") {
    auto* ptr = static_cast<const R8*>(data);
    std::vector<R8> v(ptr,ptr+sz);
    return node_value(md_array<R8,dyn_rank>(std::move(v),md_array_shape<dyn_rank>(dims)));
  } else {
    throw std_e::not_implemented_exception("make_node_value only implemented for C1, I4, I8, R4, R8");
  }
}
auto view_as_node_value(const std::string& data_type, const std::vector<I8>& dims, void* data) -> node_value {
  auto sz = std_e::cartesian_product_size(dims);
  if (data_type=="C1") {
    auto* ptr = static_cast<C1*>(data);
    std_e::span<C1> v(ptr,ptr+sz);
    return node_value(md_array_view<C1,dyn_rank>(std::move(v),md_array_shape<dyn_rank>(dims)));
  } else if (data_type=="I4") {
    auto* ptr = static_cast<I4*>(data);
    std_e::span<I4> v(ptr,ptr+sz);
    return node_value(md_array_view<I4,dyn_rank>(std::move(v),md_array_shape<dyn_rank>(dims)));
  } else if (data_type=="I8") {
    auto* ptr = static_cast<I8*>(data);
    std_e::span<I8> v(ptr,ptr+sz);
    return node_value(md_array_view<I8,dyn_rank>(std::move(v),md_array_shape<dyn_rank>(dims)));
  } else if (data_type=="R4") {
    auto* ptr = static_cast<R4*>(data);
    std_e::span<R4> v(ptr,ptr+sz);
    return node_value(md_array_view<R4,dyn_rank>(std::move(v),md_array_shape<dyn_rank>(dims)));
  } else if (data_type=="R8") {
    auto* ptr = static_cast<R8*>(data);
    std_e::span<R8> v(ptr,ptr+sz);
    return node_value(md_array_view<R8,dyn_rank>(std::move(v),md_array_shape<dyn_rank>(dims)));
  } else {
    throw std_e::not_implemented_exception("make_node_value only implemented for C1, I4, I8, R4, R8");
  }
}
// make_node_value }

} // cgns
