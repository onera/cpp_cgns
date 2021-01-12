#pragma once

#include "std_e/unit_test/doctest.hpp"
#include "pybind11/embed.h"

namespace cpp_cgns {
  namespace detail {
    extern const pybind11::scoped_interpreter python_interpreter;
    //auto init_python_interpreter() -> bool;
    //const bool python_interpreter_status = init_python_interpreter();
    // since python_interpreter_status is a global,
    // its initialization will trigger init_python_interpreter() before main
  }
}

// PYBIND_TEST_CASE presupposes that the python interpreter has been started
#define PYBIND_TEST_CASE(...) TEST_CASE(__VA_ARGS__) 
