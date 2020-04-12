cpp_cgns
========

`cpp_cgns` (C++/CGNS) offers a mapping of CGNS trees into a C++ in-memory representation, plus utilities for manipulation of C++ CGNS trees.

## Dependencies ##
`cpp_cgns` is a C++17 library. It depends on `std_e`, which is header-only (provides generic algorithms and multi-dimensional array facilities). Python interoperability (`cpp_cgns/interop` folder) requires Python and Numpy. The build process requires GCC>=8, Git>=2.14 and Cmake>=3.12. The unit tests use `doctest`.

## Build and install ##
1. `git clone https://github.com/BerengerBerthoul/std_e`
2. `cd std_e`
3. `git submodule update --init` (needed for submodule dependencies `project_utils` and `std_e`)
4. Use `cmake` to configure, build and install. The configuration step will automatically download `doctest`. If you are not familiar with `cmake` usage, an example is given in `scripts/install.sh`. By default, `cmake` will build the library, its dependencies, and a unit test executable.

## Development ##
The `cpp_cgns` repository is compatible with the development process described in `external/project_utils/doc/main.md`. It uses git submodules to ease the joint development with other repositories compatible with this organization. TL;DR: configure the git repository with `cd external/project_utils/scripts && configure_top_level_repo.sh".

## Acknowledgements ##
The design of the C++/CGNS mapping is based on python/CGNS. Tree manipulation functions are based on Cassiopee (http://elsa.onera.fr/Cassiopee/).
