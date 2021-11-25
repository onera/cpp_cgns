#pragma once


#include "std_e/buffer/polymorphic_array.hpp"
#include "std_e/multi_array/multi_array.hpp"
#include "cpp_cgns/base/data_type_variant.hpp"
#include "cpp_cgns/base/md_array.hpp"


namespace cgns {


// polymorphic_array allow the CGNS node value
// to hide memory allocation and ownership under a unique type
template<class T> using node_value_typed_array = std_e::polymorphic_array<T>;

// variant_range allows the CGNS node value
// to hide its array scalar type under a variant type
using node_value_array =
  std_e::variant_range<
    node_value_typed_array,
    dt_ref_variant,
    C1,I4,I8,R4,R8
  >;

using node_value_shape =
  std_e::dyn_shape<I8,dyn_rank>;

// a CGNS node value is actually a multi-dimensional array...
using node_value_impl =
  std_e::multi_array<
    node_value_array,
    node_value_shape
  >;


// ...but it can be constructed from various 1D or multi-D arrays
class node_value : public node_value_impl {
  private:
    struct tag_1d {};
    struct tag_multi {};
  public:
    using base = node_value_impl;

  // ctor
    node_value() = default;

    /// from 1d range
    template<class Array>
      requires
        (    std::is_rvalue_reference_v<Array&&>
          && std_e::is_1d_array<Array> )
    node_value(Array&& x)
      : node_value(x,x.size(),tag_1d{}) // extract size before moving
    {}
    //// specialization for span, since span can be efficiently passed-by-value (no need to force move)
    template<class T>
    node_value(std_e::span<T> x)
      : node_value(x,x.size(),tag_1d{}) // extract size before moving
    {}
    //// specialization for vector (for clearer error messages)
    template<class T>
    node_value(std::vector<T>&& x)
      : node_value(x,x.size(),tag_1d{}) // extract size before moving
    {}

    /// from multi array
    template<class Multi_array>
      requires
        (    std::is_rvalue_reference_v<Multi_array&&>
          && std_e::is_multi_array<Multi_array> )
    node_value(Multi_array&& x)
      : node_value(x,x.extent(),tag_multi{}) // extract extent before moving
    {}
    //// specialization for multi arrays (for clearer error messages)
    template<class M0, class M1>
    node_value(std_e::multi_array<M0,M1>&& x)
      : node_value(x,x.extent(),tag_multi{}) // extract extent before moving
    {}

    /// from range and dimensions
    template<class Array>
      requires
        (    std::is_rvalue_reference_v<Array&&>
          && std_e::is_1d_array<Array> )
    node_value(Array&& x, std::vector<I8> dims)
      : base(type_erase(std::move(x)),node_value_shape{std::move(dims)})
    {}

    /// from init list
    template<class T>
      requires is_cgns_data_type<T>
    node_value(std::initializer_list<T>&& x)
      : node_value(std::vector<T>(x.begin(),x.end()))
    {}
    template<class T>
      requires is_cgns_data_type<T>
    node_value(std::initializer_list<std::initializer_list<T>>&& x)
      : node_value(md_array<T,dyn_rank>(std::move(x)))
    {}

    /// from scalar
    template<class T>
      requires is_cgns_data_type<T>
    node_value(T x)
      : node_value({x})
    {}

    /// from string
    node_value(const std::string& x)
      : node_value(std::vector<char>(x.data(),x.data()+x.size()))
    {}
    node_value(const char* x)
      : node_value(std::string(x))
    {}

  // copy and move
    node_value(const node_value&) = delete;
    node_value& operator=(const node_value&) = delete;
    node_value(node_value&&) = default;
    node_value& operator=(node_value&&) = default;

  // data type
    auto
    data_type() const -> std::string {
      const auto& rng = underlying_range();
      if (holds_alternative<C1>(rng) && get<C1>(rng).is_null()) return "MT";
      return this->visit([]<class T>(const std_e::polymorphic_array<T>& x){ return to_string<T>(); });
    }

    template<class F> auto
    visit(F&& f) -> decltype(underlying_range().visit(FWD(f))) {
      return underlying_range().visit(FWD(f));
    }
    template<class F> auto
    visit(F&& f) const -> decltype(underlying_range().visit(FWD(f))) {
      return underlying_range().visit(FWD(f));
    }

    auto operator<=>(const node_value& x) const = default;
  private:
    template<class Array>
    node_value(Array& x, I8 sz, tag_1d)
      : base(type_erase(std::move(x)),node_value_shape{{sz}})
    {}
    template<class Array, class Multi_index>
    node_value(Array& x, Multi_index&& is, tag_multi)
      : base(type_erase(std::move(x.underlying_range())),node_value_shape{FWD(is)})
    {}


    template<class T> static auto
    type_erase(std::vector<T>&& v) -> node_value_array {
      std_e::polymorphic_array<T> parr(std::move(v));
      return node_value_array(std::move(parr));
    }
    template<class T> static auto
    type_erase(std_e::span<T>&& v) -> node_value_array {
      std_e::polymorphic_array<T> parr(std::move(v));
      return node_value_array(std::move(parr));
    }
};


// comparison {
template<class T> auto
operator==(const node_value& x, const std_e::span<T>& y) -> bool {
  if (x.rank()!=1) return false;
  return x.visit([&y]<class T0>(const std_e::polymorphic_array<T0>& x0){ return x0==y; });
}
template<class T> auto
operator==(const std_e::span<T>& x, const node_value& y) -> bool {
  return y==x;
}
template<class T> auto
operator==(const node_value& x, const std::vector<T>& y) -> bool {
  return x==std_e::make_span(y);
}
template<class T> auto
operator==(const std::vector<T>& x, const node_value& y) -> bool {
  return y==x;
}
template<class T> auto
operator!=(const node_value& x, const std_e::span<T>& y) -> bool {
  return !(x==y);
}
template<class T> auto
operator!=(const std_e::span<T>& x, const node_value& y) -> bool {
  return !(x==y);
}
template<class T> auto
operator!=(const node_value& x, const std::vector<T>& y) -> bool {
  return !(x==y);
}
template<class T> auto
operator!=(const std::vector<T>& x, const node_value& y) -> bool {
  return !(x==y);
}
// comparison }

// data_as {
template<class T> auto
data_as(node_value& x) -> T* {
  return data_as<T>(x.underlying_range());
}
template<class T> auto
data_as(const node_value& x) -> const T* {
  return data_as<T>(x.underlying_range());
}
// data_as }


// MT {
inline auto
MT() -> node_value {
  return {};
}
// MT }


/// ptr -> node_value {
auto make_node_value(const std::string& data_type, const void* data, std::vector<I8> dims) -> node_value;
auto make_non_owning_node_value(const std::string& data_type, void* data, std::vector<I8> dims) -> node_value;
/// ptr -> node_value }


/// to_string {
inline constexpr int default_threshold_to_print_whole_array = 10;
auto to_string(const node_value& x, int threshold = default_threshold_to_print_whole_array) -> std::string;
/// to_string }


} // cgns
