#pragma once

#include "pybind11/embed.h"

namespace cpp_cgns {
  namespace detail {
    extern const pybind11::scoped_interpreter python_interpreter;
  }
}
