********************
Welcome to C++/CGNS!
********************

**cpp_cgns** (C++/CGNS) offers a mapping of CGNS trees into a C++ in-memory representation, utilities for manipulation of C++/CGNS trees, and conversions with Python/CGNS trees.

Installation
------------
**cpp_cgns** is a C++20 library. It depends on **std_e** (generic algorithms and multi-dimensional array facilities). Python interoperability (:code:`cpp_cgns/interop` folder) requires Python, Numpy and uses Pybind11 for wrapping. The build process requires GCC>=8, Git>=2.14 and Cmake>=3.14. The unit tests use `doctest`.

1. :code:`git clone https://github.com/BerengerBerthoul/cpp_cgns`
2. :code:`cd cpp_cgns`
3. :code:`git submodule update --init` (needed for submodule dependencies **project_utils** and **std_e**)
4. Use **cmake** to configure, build and install. The configuration step will automatically download other dependencies (**doctest** and **pybind11**).


Table of contents
-----------------

.. toctree::
  :maxdepth: 6
  :caption: Reference:

  quick_start
  user_manual/user_manual
  installation
  design_documents/design_documents

.. toctree::
  :maxdepth: 6
  :caption: Appendix:

  releases/release_notes
  license
  todo
