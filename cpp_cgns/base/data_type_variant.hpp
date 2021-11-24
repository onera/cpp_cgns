#pragma once


#include "cpp_cgns/base/data_type.hpp"
#include "std_e/data_structure/variant_range.hpp"


namespace cgns {


template<class... Ts>
// requires Ts...==C1,I4,I8,R4,R8;
class dt_ref_variant : public std_e::reference_variant<Ts...> {
  public:
    using base = std_e::reference_variant<Ts...>;
    using base::base;

  // assignement
    // exact same type
    template<class T>
      requires std_e::exactly_once<T,Ts...>
    auto
    operator=(const T& x) -> dt_ref_variant& {
      base& self_as_base = *this;
      self_as_base = x;
      return *this;
    }
    /// handle safe cases 
    constexpr
    dt_ref_variant& operator=(const I4& x) {
      static_assert(std_e::exactly_once<I4,Ts...>); // ensures non-const versions
      if (holds_alternative<I8>(*this)) {
        get<I8>(*this) = x;
      } else {
        get<I4>(*this) = x;
      }
      return *this;
    }
    constexpr
    dt_ref_variant& operator=(const R4& x) {
      static_assert(std_e::exactly_once<R4,Ts...>); // ensures non-const versions
      if (holds_alternative<R8>(*this)) {
        get<R8>(*this) = x;
      } else {
        get<R4>(*this) = x;
      }
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


using dt_ref_var = dt_ref_variant<C1,I4,I8,R4,R8>;

template<class T>
  requires std_e::exactly_once<T,C1,I4,I8,R4,R8>
constexpr auto
operator==(const dt_ref_var& x, T y) -> bool {
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
template<class T>
  requires std_e::exactly_once<T,C1,I4,I8,R4,R8>
constexpr auto
operator==(T x, const dt_ref_var& y) -> bool {
  return y==x;
}
template<class T>
  requires std_e::exactly_once<T,C1,I4,I8,R4,R8>
constexpr auto
operator!=(const dt_ref_var& x, T y) -> bool {
  return !(x==y);
}
template<class T>
  requires std_e::exactly_once<T,C1,I4,I8,R4,R8>
constexpr auto
operator!=(T x, const dt_ref_var& y) -> bool {
  return !(x==y);
}


} // cgns
