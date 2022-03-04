#include "cpp_cgns/base/node_value.hpp"


#include "std_e/multi_array/utils.hpp"
#include "cpp_cgns/base/node_value_conversion.hpp"
#include "cpp_cgns/dispatch.hpp"


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
  return
    dispatch_on_data_type(
      x.data_type(),
      [&x]<class T>(T){ return to_string(view_as_md_array<T,dyn_rank>(x)); }
    );
}

auto to_string(const node_value& x, int threshold) -> std::string {
  if (x.data_type()=="MT") return "MT";
  if (x.data_type()=="C1") return std::string((const char*)x.data(),x.extent(0));
  if (std_e::cartesian_product_size(x.extent())<=threshold) return x.data_type()+to_complete_string(x);
  else return "{"+x.data_type()+":"+dims_to_string(x.extent())+"}";
}
// to_string }


// make_node_value {
constexpr auto
make_node_value_impl = []<class T>(T, const void* data, std::vector<I8> dims) -> node_value {
  auto sz = std_e::cartesian_product_size(dims);
  auto* ptr = static_cast<const T*>(data);
  std::vector<T> rng(ptr,ptr+sz);
  return node_value(md_array<T,dyn_rank>(std::move(rng),md_array_shape<dyn_rank>(std::move(dims))));
};

constexpr auto
make_non_owning_node_value_impl = []<class T>(T, void* data, std::vector<I8> dims) -> node_value {
  auto sz = std_e::cartesian_product_size(dims);
  auto* ptr = static_cast<T*>(data);
  std_e::span<T> rng(ptr,ptr+sz);
  return node_value(md_array_view<T,dyn_rank>(std::move(rng),md_array_shape<dyn_rank>(std::move(dims))));
};

auto
make_node_value(const std::string& data_type, const void* data, std::vector<I8> dims) -> node_value {
  return
    dispatch_on_data_type(
      data_type,
      make_node_value_impl,
        data,
        std::move(dims));
}
auto
make_non_owning_node_value(const std::string& data_type, void* data, std::vector<I8> dims) -> node_value {
  return
    dispatch_on_data_type(
      data_type,
      make_non_owning_node_value_impl,
        data,
        std::move(dims));
}
// make_node_value }

} // cgns
