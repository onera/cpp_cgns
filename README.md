C++/CGNS
========

`cpp_cgns` (C++/CGNS) offers a mapping of CGNS trees into a C++ in-memory representation, plus utilities for manipulation of C++ CGNS trees.

The documentation can be found [here](iris.onera.net/cpp_cgns/index.html)

## Dependencies ##
`cpp_cgns` is a C++ library. It depends on `std_e` (provides generic algorithms and multi-dimensional array facilities). Python interoperability (`cpp_cgns/interop` folder) requires Python and Numpy.

## Build and install ##
1. `git clone https://gitlab.onera.net/numerics/mesh/cpp_cgns`
2. `cd cpp_cgns`
3. `git submodule update --init` (needed to retrieve submodules)
4. Use `cmake` to configure, build and install

## Acknowledgements ##
The design of the C++/CGNS mapping is based on Python/CGNS. Tree manipulation functions are based on [Cassiopee](http://elsa.onera.fr/Cassiopee/).

## License ##
`cpp_cgns` is available under the [MPL-2.0 license](https://mozilla.org/MPL/2.0/).
