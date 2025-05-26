#pragma once


#include "std_e/buffer/polymorphic_array.hpp"
#include "std_e/multi_array/multi_array.hpp"
#include "std_e/future/dynarray.hpp"
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
    scalar_ref_variant,
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
    /// special
    node_value() = default;
    node_value(const node_value&) = delete;
    node_value& operator=(const node_value&) = delete;
    node_value(node_value&&) = default;
    node_value& operator=(node_value&&) = default;

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
    //// specialization for dynarray (for clearer error messages)
    template<class T>
    node_value(std_e::dynarray<T>&& x)
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
      requires is_data_type<T>
    node_value(std::initializer_list<T>&& x)
      : node_value(std::vector<T>(x.begin(),x.end()))
    {}
    template<class T>
      requires is_data_type<T>
    node_value(std::initializer_list<std::initializer_list<T>>&& x)
      : node_value(md_array<T,dyn_rank>(std::move(x)))
    {}

    /// from scalar
    template<class T>
      requires is_data_type<T>
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

  // data type
    template<class T>
      requires is_data_type<T>
        friend auto
    holds_alternative(const node_value& x) -> bool;
    auto
    data_type() const -> std::string {
      const auto& rng = underlying_range();
      if (std_e::holds_alternative<C1>(rng) && std_e::get<C1>(rng).is_null()) return "MT";
      return this->visit([]<class T>(const std_e::polymorphic_array<T>&){ return to_string<T>(); });
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


    template<class Array>
      requires (std::is_rvalue_reference_v<Array&&>)
       static auto
    type_erase(Array&& arr) -> node_value_array {
      using T = typename Array::value_type;
      std_e::polymorphic_array<T> parr(std::move(arr));
      return node_value_array(std::move(parr));
    }
};

template<class T>
  requires is_data_type<T>
    auto
holds_alternative(const node_value& x) -> bool {
  const auto& rng = x.underlying_range();
  return std_e::holds_alternative<T>(rng);
}

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


/// types that can be used to create a `node_value` {
// We could give any range for creating `node_value` objects
// Then the range will be type-erased and stored in the `node_value`
// For now, this is only done for
//   - node_value&&, std::vector&&, std_e::dynarray&&, md_array&& (we force a `std::move` to prevent needless copies)
//   - std_e::span (copies are very cheap)
//   - md_array_view (copying dimensions is not free, but we don't prevent the user if he doesn't `std::move`)
// While we can make things more general (with owning/non-owning array concepts), we don't need this for now

template<class Arr> struct _is_span : std::false_type {};
template<class T> struct _is_span<std_e::span<T>> : std::true_type {};

template<class Arr> struct _is_dynarray : std::false_type {};
template<class T, class A> struct _is_dynarray<std_e::dynarray<T,A>> : std::true_type {};

template<class Arr> struct _is_vector : std::false_type {};
template<class T, class A> struct _is_vector<std::vector<T,A>> : std::true_type {};

template<class Arr> struct _is_md_array : std::false_type {};
template<class T, int rank> struct _is_md_array<md_array<T,rank>> : std::true_type {};

template<class Arr> struct _is_md_array_view : std::false_type {};
template<class T, int rank> struct _is_md_array_view<md_array_view<T,rank>> : std::true_type {};

template<class Arr> constexpr auto
_movable_to_node_value_impl() -> bool {
  using Decayed = std::decay_t<Arr>;
  constexpr bool is_node_value    = std::is_same_v<Decayed, node_value>;
  constexpr bool is_span          = _is_span         <Decayed>::value;
  constexpr bool is_dynarray      = _is_dynarray     <Decayed>::value;
  constexpr bool is_vector        = _is_vector       <Decayed>::value;
  constexpr bool is_md_array      = _is_md_array     <Decayed>::value;
  constexpr bool is_md_array_view = _is_md_array_view<Decayed>::value;

  constexpr bool is_non_owning_array = is_span || is_md_array_view;
  constexpr bool is_owning_array = is_node_value || is_dynarray || is_vector || is_md_array;

  if constexpr (is_non_owning_array) { return true; }

  if constexpr (is_owning_array) {
    return not std::is_lvalue_reference_v<Arr>; // don't allow implicit copies (force move)
  }

  return false;
}

template<class Arr>
constexpr bool _movable_to_node_value = _movable_to_node_value_impl<Arr>();
/// types that can be used to create a `node_value` }


} // cgns
