#pragma once


#include "cpp_cgns/base/data_type.hpp"
#include "std_e/data_structure/variant_range.hpp"


namespace cgns {


template<class... Ts>
// requires Ts...==C1,I4,I8,R4,R8;
class scalar_ref_variant : public std_e::reference_variant<Ts...> {
  public:
    using base = std_e::reference_variant<Ts...>;
    using base::base;

  // utility
    auto
    data_type() const -> std::string {
      const base& self_as_base = *this;
      return visit([]<class T>(const T&){ return to_string<T>(); } , self_as_base);
    }

  // assignement
    /// I4 can be promoted to I8
    constexpr
    scalar_ref_variant& operator=(const I4& x) {
      static_assert(std_e::exactly_once<I4,Ts...>); // ensures non-const versions
      if (holds_alternative<I8>(*this)) {
        get<I8>(*this) = x;
      } else if (holds_alternative<I4>(*this)) {
        get<I4>(*this) = x;
      } else {
        throw cgns_exception("Narrowing conversion from "+to_string<I4>()+" to "+data_type());
      }
      return *this;
    }
    /// R4 can be promoted to R8
    constexpr
    scalar_ref_variant& operator=(const R4& x) {
      static_assert(std_e::exactly_once<R4,Ts...>); // ensures non-const versions
      if (holds_alternative<R8>(*this)) {
        get<R8>(*this) = x;
      } else if (holds_alternative<R4>(*this)) {
        get<R4>(*this) = x;
      } else {
        throw cgns_exception("Narrowing conversion from "+to_string<R4>()+" to "+data_type());
      }
      return *this;
    }
    /// else we require the same type
    template<class T>
      requires std_e::exactly_once<T,Ts...>
    constexpr
    scalar_ref_variant& operator=(const T& x) {
      base& self_as_base = *this;
      if (!holds_alternative<T>(self_as_base)) {
        throw cgns_exception("Narrowing conversion from "+to_string<T>()+" to "+data_type());
      }
      self_as_base = x;
      return *this;
    }

    // conversions to value: only to exact type or I4->I8 and R4->R8
    constexpr
    operator C1() const {
      return get<C1>(*this);
    }
    constexpr
    operator I4() const {
      return get<I4>(*this);
    }
    constexpr
    operator I8() const {
      if (holds_alternative<I8>(*this)) {
        return get<I8>(*this);
      } else {
        return get<I4>(*this);
      }
    }
    constexpr
    operator R4() const {
      return get<R4>(*this);
    }
    constexpr
    operator R8() const {
      if (holds_alternative<R8>(*this)) {
        return get<R8>(*this);
      } else {
        return get<R4>(*this);
      }
    }
};


using scalar_ref = scalar_ref_variant<C1,I4,I8,R4,R8>;
using scalar_const_ref = scalar_ref_variant<const C1,const I4,const I8,const R4,const R8>;
template<class T> concept Scalar_ref = std::is_same_v<T,scalar_ref> || std::is_same_v<T,scalar_const_ref>;


struct scalar {}; // Not used for now...
template<class T> concept Scalar = std::is_same_v<T,scalar> || Scalar_ref<T>;


template<Scalar S, Data_type T>
constexpr auto
operator==(const S& x, T y) -> bool {
  if (holds_alternative<T>(x)) { // if same underlying type, cast and compare
    return T(x)==y;
  } else if (holds_alternative<R4>(x) && std::is_same_v<T,R8>) { // compare R4 and R8
    return R4(x)==y;
  } else if (holds_alternative<R8>(x) && std::is_same_v<T,R4>) { // compare R8 and R4
    return R8(x)==y;
  } else if (holds_alternative<I4>(x) && std::is_same_v<T,I8>) { // compare I4 and I8
    return I4(x)==y;
  } else if (holds_alternative<I8>(x) && std::is_same_v<T,I4>) { // compare I8 and I4
    return I8(x)==y;
  } else { // compare a floating point and an integer, or worse: consider not equal
    return false;
  }
}
template<Scalar S, Data_type T>
constexpr auto
operator==(T x, const S& y) -> bool {
  return y==x;
}
template<Scalar S, Data_type T>
constexpr auto
operator!=(const S& x, T y) -> bool {
  return !(x==y);
}
template<Scalar S, Data_type T>
constexpr auto
operator!=(T x, const S& y) -> bool {
  return !(x==y);
}


} // cgns
