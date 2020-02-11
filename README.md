## Introduction ##
`cpp_cgns` offers a mapping of CGNS trees into a C++ in-memory representation, plus utilities for manipulation of C++ CGNS trees.

## Dependencies ##
`cpp_cgns` is a C++14 library. It depends on `std_e`, which is header-only (provides generic algorithms and multi-dimensional array facilities). Python interoperability requires headers from Python and Numpy which are grouped in the `python_c_api` folder. The unit tests use the library `doctest`

## Install ##
1. Create a sub-folder `extern` in this folder. Inside `extern`, make a copy or link the dependency repositories, i.e. `std_e` and `doctest`.
2. Go to `install`
3. Build the sources with Cmake. An example is given in file `run.sh`. By default, the unit tests will also be built.
  * Use `cmake --build ../build --target cpp_cgns` to only build the library

## Acknowledgements ##
The design of the C++/CGNS mapping is based on python/CGNS. Tree manipulation functions are based on Cassiopee (http://elsa.onera.fr/Cassiopee/).
