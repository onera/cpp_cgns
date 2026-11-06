#pragma once


#include "std_e/base/msg_exception.hpp"
// TODO use assertion_exception (that may be renamed to stacktrace_exception)


namespace cgns {


class cgns_exception : public std_e::msg_exception {
  public:
    using base = std_e::msg_exception;
    using base::base;
};


} // cgns
