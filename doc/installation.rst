.. _installation:

Installation
============

Dependencies
------------

**C++/CGNS** is a C++20 library. It depends on **std_e**, (provides generic algorithms and multi-dimensional array facilities). Python interoperability (`cpp_cgns/interop` folder) requires Python, Numpy and Pybind11.

The build process requires:

* Cmake >= 3.14
* GCC >= 8

The unit tests use `doctest`.

The documentation build requires:

* Doxygen >= 1.8.19
* Breathe >= 4.15 (python package)
* Sphinx >= 3.00 (python package)

Build and install
-----------------

1. `git clone https://github.com/BerengerBerthoul/cpp_cgns`
2. `cd cpp_cgns`
3. `git submodule update --init` (needed for submodule dependencies `project_utils` and `std_e`)
4. Use `cmake` to configure, build and install. The configuration step will automatically download `doctest` and `Pybind11` if no suitable version is found. By default, `cmake` will build the library, its dependencies, and a unit test executable.
5. Tests can be launched with `ctest`. The unit tests binary can be launched with `./test/cpp_cgns_doctest_unit_tests`
