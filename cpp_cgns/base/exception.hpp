#pragma once


#include "std_e/base/msg_exception.hpp"


namespace cgns {


class cgns_exception : public std_e::msg_exception {
  public:
    using base = std_e::msg_exception;
    using base::base;
};


} // cgns
