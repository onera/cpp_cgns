#pragma once

#include "std_e/unit_test/doctest.hpp"
#include "cpp_cgns/interop/embed_python/interpreter.hpp"

// PYBIND_TEST_CASE presupposes that the python interpreter has been started
#define PYBIND_TEST_CASE(...) TEST_CASE(__VA_ARGS__)
