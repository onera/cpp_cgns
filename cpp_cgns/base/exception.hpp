#pragma once


#include "std_e/base/stacktrace_exception.hpp"


namespace cgns {


class cgns_exception : public std_e::stacktrace_exception {
  public:
    using base = std_e::stacktrace_exception;
    using base::base;
};


} // cgns
