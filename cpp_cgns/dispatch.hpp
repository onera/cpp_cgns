#pragma once


#include "cpp_cgns/cgns.hpp"


namespace cgns {


template<class F> auto
dispatch_I4_I8(const std::string& type, F f) {
  if (type=="I4") {
    return f(I4{});
  }
  if (type=="I8") {
    return f(I8{});
  }
  throw cgns_exception("dispatch_I4_I8 expects an integer data_type, but got "+type);
}


template<class F> auto
dispatch_on_data_type(const std::string& type, F f) {
  if (type=="C1") {
    return f(C1{});
  }
  if (type=="I4") {
    return f(I4{});
  }
  if (type=="I8") {
    return f(I8{});
  }
  if (type=="R4") {
    return f(R4{});
  }
  if (type=="R8") {
    return f(R8{});
  }
  throw cgns_exception("dispatch_on_data_type expects a C1, I4, I8, R4 or R8 data_type, but got "+type);
}




inline auto _data_type(const tree      & t) -> std::string { return t.value.data_type(); }
inline auto _data_type(const node_value& x) -> std::string { return x.      data_type(); }

template<class F, class T> auto
// requires T is [const]tree or [const]node_value
dispatch_I4_I8(F f, T& x) {
  auto type = _data_type(x);
  return dispatch_I4_I8(
    type,
    [f,&x](auto type){ return f(type,x); }
  );
}

template<class F, class T> auto
// requires T is [const]tree or [const]node_value
dispatch_on_data_type(F f, T& x) {
  auto type = _data_type(x);
  return dispatch_on_data_type(
    type,
    [f,&x](auto type){ return f(type,x); }
  );
}


} // cgns
