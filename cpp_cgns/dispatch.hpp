#pragma once


#include "cpp_cgns/cgns.hpp"
#include "cpp_cgns/exception.hpp"


namespace cgns {


template<class F, class Tree> auto
// requires Tree is tree or const tree
dispatch_I4_I8(F f, Tree& t) {
  auto type = t.value.data_type;
  if (type=="I4") {
    return f(I4{},t);
  }
  if (type=="I8") {
    return f(I8{},t);
  }
  throw cgns_exception("dispatch_I4_I8 expects an integer data_type");
}


} // cgns
