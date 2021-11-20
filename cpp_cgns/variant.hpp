#pragma once


#include "std_e/data_structure/variant_range.hpp"


namespace cgns {


template<class... Ts>
// requires remove_const<Ts>==[C1|I4|I8|R4|R8];
class dt_ref_variant : public reference_variant<Ts...> {
  using base = reference_variant<Ts...>;
  using base::base;

  // assignement: handle safe cases (note: exact type assignment already implemented in reference_variant)
  operator=(const I4& x) -> dt_ref_variant& {
    static_assert(exactly_once<I4,Ts...>); // ensures non-const versions
    if (holds_alternative<I8>(*this)) {
      get<I8>(*this) = x;
    } else {
      get<I4>(*this) = x;
    }
    return *this;
  }
  operator=(const R4& x) -> dt_ref_variant& {
    static_assert(exactly_once<R4,Ts...>); // ensures non-const versions
    if (holds_alternative<R8>(*this)) {
      get<R8>(*this) = x;
    } else {
      get<R4>(*this) = x;
    }
    return *this;
  }

  // conversions to value: only to exact type or I4->I8 and R4->R8
  operator C1() const {
    return get<C1>(*this);
  }
  operator I4() const {
    return get<I4>(*this);
  }
  operator I8() const {
    if (holds_alternative<I8>(*this)) {
      return get<I8>(*this);
    } else if (holds_alternative<I4>(*this)) {
      return get<I4>(*this);
    }
  }
  operator R4() const {
    return get<R4>(*this);
  }
  operator R8() const {
    if (holds_alternative<R8>(*this)) {
      return get<R8>(*this);
    } else if (holds_alternative<R4>(*this)) {
      return get<R4>(*this);
    }
  }

  // conversions to non-const ref: only if the ref type is non-const
  operator C1&() {
    static_assert(exactly_once<C1,Ts...>);
    return get<C1>(*this);
  }
  operator I4&() {
    static_assert(exactly_once<I4,Ts...>);
    return get<I4>(*this);
  }
  operator I8&() {
    static_assert(exactly_once<I8,Ts...>);
    return get<I8>(*this);
  }
  operator R4&() {
    static_assert(exactly_once<R4,Ts...>);
    return get<R4>(*this);
  }
  operator R8&() {
    static_assert(exactly_once<R8,Ts...>);
    return get<R8>(*this);
  }

  // conversions to const ref
  operator const C1&() const {
    return get<C1>(*this);
  }
  operator const I4&() const {
    return get<I4>(*this);
  }
  operator const I8&() const {
    return get<I8>(*this);
  }
  operator const R4&() const {
    return get<R4>(*this);
  }
  operator const R8&() const {
    return get<R8>(*this);
  }
};


//template<class... Ts>
//class dt_ref_variant;
//
//template<>
//class dt_ref_variant<C1,I4,I8,R4,R8> : public dt_ref_variant<C1,I4,I8,R4,R8> {
//  using base = dt_ref_variant<C1,I4,I8,R4,R8>;
//  using base::base;
//};
//template<>
//class dt_ref_variant<const C1,const I4,const I8,const R4,const R8> : public dt_ref_variant<const C1,const I4,const I8,const R4,const R8> {
//  using base = dt_ref_variant<const C1,const I4,const I8,const R4,const R8>;
//  using base::base;
//};


using dt_variant_range = variant_range<dt_ref_variant, C1,I4,I8,R4,R8>;


} // cgns
