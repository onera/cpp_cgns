#pragma once


#include "std_e/buffer/polymorphic_array.hpp"
#include "std_e/multi_array/multi_array.hpp"
#include "cpp_cgns/base/data_type.hpp"

#include "std_e/buffer/polymorphic_buffer.hpp" // TODO DEL
#include "std_e/buffer/buffer_vector.hpp" // TODO DEL


namespace cgns {


//// polymorphic_array allow the CGNS node value
//// to hide memory allocation and ownership under a unique type
//template<class T> using node_value_underlying_memory = std_e::polymorphic_array<T>;
//
//// variant_range allows the CGNS node value
//// to hide its array scalar type under a variant type
//using node_value_underlying_range =
//  variant_range<
//    node_value_underlying_memory,
//    C1,I4,I8,R4,R8
//  >;
//
//using node_value_shape =
//  std_e::dyn_shape<I8,std_e::dynamic_size>;
//
//// a CGNS node value is actually a multi-dimensional array...
//using node_value_impl =
//  std_e::multi_array<
//    node_value_underlying_range,
//    node_value_shape
//  >;
//
//
//// ...but it can be constructed from various 1D or multi-D arrays
//class node_value : public node_value_impl {
//  private:
//    struct tag_1d {};
//    struct tag_multi {};
//  public:
//    using base = node_value_impl;
//
//  // ctor
//    node_value()
//      : base(null_array<C1>{},node_value_shape{{}})
//    {}
//
//    template<class Array>
//      requires
//        (   std::is_rvalue_reference_v<Array&&>
//         && is_1d_array<Array> )
//    node_value(Array&& x)
//      : base(x,x.size(),tag_1d{}) // extract size before moving
//    {}
//
//    template<class Multi_array>
//      requires
//        (   std::is_rvalue_reference_v<Multi_array&&>
//         && is_multi_array<Multi_array> )
//    node_value(Multi_array&& x)
//      : base(x,x.extent(),tag_multi{}) // extract extent before moving
//    {}
//
//  // release the memory: it is now the responsibility of the caller to delete it
//    //using polymorphic_base = typename node_value_underlying_memory::polymorphic_base;
//    auto underlying_variant() -> auto& {
//      return this->underlying_range().underlying_variant();
//    }
//
//  private:
//    template<class Array>
//    node_value(Array& x, I8 sz, tag_1d)
//      : base(std::move(x),node_value_shape{{sz}})
//    {}
//    template<class Array, class Multi_index>
//    node_value(Array& x, Multi_index&& is, tag_multi)
//      : base(std::move(x),node_value_shape{FWD(is)})
//    {}
//};

struct node_value {
  std::string data_type; // TODO enum
  std::vector<I8> dims;
  std_e::polymorphic_buffer buffer;

  node_value() = default;
  node_value(node_value&&) = default;
  node_value& operator=(node_value&&) = default;
  node_value(const node_value&) = delete;
  node_value& operator=(const node_value&) = delete;

  node_value(std::string dt, std::vector<I8> dims, std_e::polymorphic_buffer buf)
    : data_type(std::move(dt))
    , dims(std::move(dims))
    , buffer(std::move(buf))
  {}
};
inline auto
data(node_value& x) -> void* {
  return x.buffer.data();
}
inline auto
data(const node_value& x) -> const void* {
  return x.buffer.data();
}

} // cgns
