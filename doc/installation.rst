.. _installation:

Installation
============

Dependencies
------------

`cpp_cgns` is a C++17 library. It depends on `std_e`, which is header-only (provides generic algorithms and multi-dimensional array facilities). Python interoperability (`cpp_cgns/interop` folder) requires Python and Numpy.

The build process requires:

* Cmake >= 3.12
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
4. Use `cmake` to configure, build and install. The configuration step will automatically download `doctest`. If you are not familiar with `cmake` usage, an example is given in `scripts/install.sh`. By default, `cmake` will build the library, its dependencies, and a unit test executable.
