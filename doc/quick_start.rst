.. _quick_start:

.. currentmodule:: cpp_cgns

Quick start
===========

.. contents:: :local:

Introduction
------------

**C++/CGNS** offers a mapping of CGNS trees into a C++ in-memory representation, utilities for manipulation of these trees, and shared-memory translation to Python/CGNS trees.

The following documentation supposes basic knowledge of `CGNS <https://cgns.github.io/index.html>`_ (in particular, the `node structure <https://cgns.github.io/CGNS_docs_current/filemap/general.html>`_ and the `SIDS <https://cgns.github.io/CGNS_docs_current/sids/index.html>`_).

Installation
------------

See :ref:`installation`.

Note
----

Everything is defined within the :cpp:`cgns` namespace.

In the examples below, when there is no ambiguity, we will assume that we are using the :cpp:`std` and :cpp:`cgns` namespaces:

.. code:: c++

  using namespace std;
  using namespace cgns;

.. note::

  Most of the code snippets presented here are actually directly extracted from **cpp_cgns** unit tests. A :cpp:`TEST_CASE` allows to define a unit test. Each :cpp:`SUBCASE` is a subsection of the unit test. The :cpp:`CHECK` command allows to easily see what the expected result is supposed to be.

.. .. sectnum::

.. _cpp_cgns_mapping:

C++/CGNS mapping
----------------

The :cpp:`cgns::tree` class
^^^^^^^^^^^^^^^^^^^^^^^^^^^

A :cpp:`cgns::tree` is composed of:

* a name of type :cpp:`std::string`
* a label of type :cpp:`std::string`
* a multi-dimensional array of type :cpp:`cgns::node_value`
* a list of children of type :cpp:`cgns::tree_children`

A :cpp:`cgns::tree` can be built and queried with the following base API:

.. literalinclude:: /../cpp_cgns/base/test/tree.test.cpp
  :language: C++
  :start-after: [Sphinx Doc] cgns::tree first example {
  :end-before: [Sphinx Doc] cgns::tree first example }

Adding a child
~~~~~~~~~~~~~~~

We will see later the various possible manipulations of a :cpp:`cgns::tree`, but a very common one is to add a child:

.. literalinclude:: /../cpp_cgns/base/test/tree.test.cpp
  :language: C++
  :start-after: [Sphinx Doc] adding a child {
  :end-before: [Sphinx Doc] adding a child }

Note that the :cpp:`emplace_child(t,c)` function actually returns a reference to the emplaced child:

.. code:: c++

  tree& c = emplace_child(t,std::move(sub_t));
  CHECK( name(c) == "SubNode" );

To append several children at once, use :cpp:`emplace_children(t,cs)`

Move, but do not copy
~~~~~~~~~~~~~~~~~~~~~

In the previous example, :cpp:`sub_t` was moved as a child of :cpp:`t`. This is because trees may hold large arrays of data, and hence copying them would be inefficient. A CGNS tree is intended to be viewed as a simulation database. So when we create a CGNS node, we want to move it as a child of another tree.

Because of this model, the :cpp:`cgns::tree` copy constructor and copy assignment are deleted. It prevents the accidental, unneeded copy of big arrays. Tree copy is seldom (if ever) needed anyways. So you have to use :cpp:`std::move` (or construct in-place). Just remember to not use the object after it has been moved:

.. code:: c++

  emplace_child(t,std::move(sub_t));
  // WARNING `sub_t` is not a valid object anymore!
  // You have to query the tree to retrieve information

String representation
~~~~~~~~~~~~~~~~~~~~~

As most objects of the library, a :cpp:`cgns::tree` has an associated :cpp:`to_string` function that can be use the display the tree:

.. code:: c++

  #include "cpp_cgns/cgns.hpp"
  #include <iostream>
  std::cout << to_string(t);


.. code:: text

  Base, [3,3], CGNSBase_t
    Z0, [8,1,0], Zone_t
    Z1, [12,2,0], Zone_t

To avoid very long representations, big arrays are only represented by their dimensions, not their coefficients.


Tree comparisons
~~~~~~~~~~~~~~~~

Trees can be compared for value equality through the usual :cpp:`operator==`. Also, :cpp:`same_tree_structure(t0,t1)` can be used to check if trees are equal irrespective their array values.

.. literalinclude:: /../cpp_cgns/base/test/tree.test.cpp
  :language: C++
  :start-after: [Sphinx Doc] tree equality {
  :end-before: [Sphinx Doc] tree equality }

The :cpp:`cgns::node_value` class
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Note: The CGNS node structure can be found `here <https://cgns.github.io/CGNS_docs_current/filemap/general.html>`_.

CGNS data types
~~~~~~~~~~~~~~~

The CGNS standard defines possible 5 types for array coefficients, and they are defined in the library as aliases to C++ types:

.. literalinclude:: /../cpp_cgns/base/data_type.hpp
  :language: C++
  :start-after: [Sphinx Doc] cgns data types {
  :end-before: [Sphinx Doc] cgns data types }

CGNS node value
~~~~~~~~~~~~~~~

The :cpp:`node_value` of a tree is a multi-dimensional array. It can be constructed from various types.

Mono-dimensional arrays
"""""""""""""""""""""""

A mono-dimensional array can be constructed by any range whose memory is contiguous (and hence that has a :cpp:`data()` method).

*  Example for a :cpp:`std::vector<R8>`:

.. literalinclude:: /../cpp_cgns/base/test/node_value.test.cpp
  :language: C++
  :start-after: [Sphinx Doc] node_value from vec R8 {
  :end-before: [Sphinx Doc] node_value from vec R8 }

The vector is moved into the :cpp:`node_value`. This means the lifetime of the data is bound to that of the :cpp:`node_value`.

*  Example for a :cpp:`std_e::span<I4>`:

.. literalinclude:: /../cpp_cgns/base/test/node_value.test.cpp
  :language: C++
  :start-after: [Sphinx Doc] node_value from span I4 {
  :end-before: [Sphinx Doc] node_value from span I4 }

Here, the span is also held by the :cpp:`node_value`, but since the span does not own its memory, the lifetime of the data is that of the original container (here, the vector :cpp:`v`).

*  For arrays of type :cpp:`C1`, a :cpp:`node_value` can be constructed from a :cpp:`std::vector<char>`, but a :cpp:`std::string` feels more natural:

.. literalinclude:: /../cpp_cgns/base/test/node_value.test.cpp
  :language: C++
  :start-after: [Sphinx Doc] node_value from string {
  :end-before: [Sphinx Doc] node_value from string }

*  As a shortcut, using a :cpp:`std::initalizer_list` to construct a :cpp:`node_value` will have the exact same effect as constructing it from a :cpp:`std::vector`:

.. literalinclude:: /../cpp_cgns/base/test/node_value.test.cpp
  :language: C++
  :start-after: [Sphinx Doc] node_value from init list {
  :end-before: [Sphinx Doc] node_value from init list }


Multi-dimensional arrays
""""""""""""""""""""""""

The principle of constructing multi-dimensional arrays is very similar.

* Example with an :cpp:`cgns::md_array` (owning memory):

.. literalinclude:: /../cpp_cgns/base/test/node_value.test.cpp
  :language: C++
  :start-after: [Sphinx Doc] node_value from md_array {
  :end-before: [Sphinx Doc] node_value from md_array }

* Example with a 2D :cpp:`std::initializer_list` (syntactic sugar for exactly the same as above):

.. literalinclude:: /../cpp_cgns/base/test/node_value.test.cpp
  :language: C++
  :start-after: [Sphinx Doc] node_value from multi dim init_list {
  :end-before: [Sphinx Doc] node_value from multi dim init_list }

* Example with an :cpp:`cgns::md_array_view` (non-owning memory):

.. literalinclude:: /../cpp_cgns/base/test/node_value.test.cpp
  :language: C++
  :start-after: [Sphinx Doc] node_value from md_array_view {
  :end-before: [Sphinx Doc] node_value from md_array_view }

* A multi-dimensional array can also be obtained by simply reshaping a mono- or multi-dimensional array:

.. literalinclude:: /../cpp_cgns/base/test/node_value.test.cpp
  :language: C++
  :start-after: [Sphinx Doc] node_value reshape {
  :end-before: [Sphinx Doc] node_value reshape }

Note that CGNS arrays are Fortran-ordered.


Empty arrays
""""""""""""

A default-initialized :cpp:`node_value` will be of rank 0 and have the special data_type :cpp:`"MT"`. The :cpp:`MT()` function is a syntactic sugar to build an empty :cpp:`node_value`.

.. literalinclude:: /../cpp_cgns/base/test/node_value.test.cpp
  :language: C++
  :start-after: [Sphinx Doc] empty node_value {
  :end-before: [Sphinx Doc] empty node_value }

How does it work underneath?
""""""""""""""""""""""""""""

CGNS arrays handle three kinds of variability:

1. multiple ranks and extents
2. multiple coefficient types
3. multiple construction methods and memory ownership

Dynamic ranks
'''''''''''''

The :cpp:`node_value` class inherits from :cpp:`std_e::multi_array`, which supports dynamic-sized ranks.

Multiple scalar types
'''''''''''''''''''''

A multi-dimensional array is just syntacic sugar built on top of a mono-dimensional range.

The range underneath :cpp:`node_value` is a :cpp:`std_e::variant_range`. In a nutshell, for a range :cpp:`Rng`, a :cpp:`std_e::variant_range` is a wrapper around a :cpp:`std::variant<Rng<C1>,Rng<I4>,Rng<I8>,Rng<R4>,Rng<R8>>`. A structure like :cpp:`std::variant<Rng<T>>` is quite efficient because even if the whole range can have multiple possible coefficient types (i.e. :cpp:`C1`, :cpp:`I4`, :cpp:`I8`, :cpp:`R4` or :cpp:`R8`), however its individual elements are all of that same type.

Coefficients of a :cpp:`node_value` are of type :cpp:`scalar_ref`.

.. literalinclude:: /../cpp_cgns/base/test/node_value.test.cpp
  :language: C++
  :start-after: [Sphinx Doc] node_value scalar_ref {
  :end-before: [Sphinx Doc] node_value scalar_ref }


:cpp:`scalar_ref` is a variant reference: something that behaves like a :cpp:`C1&`, a :cpp:`I4&`, a :cpp:`I8&`, a :cpp:`R4&`, or a :cpp:`R8&`. Assigning it the wrong type is an error, since the type is imposed by the origin range of the reference, and individual elements of the range can't change type.

Multiple construction methods
'''''''''''''''''''''''''''''

We talked about :cpp:`std_e::variant_range<Rng<T>...>` and how we handled the five possible types for T. However, we did not specify what is the actual type of :cpp:`Rng<T>`. This is because there is another difficulty here. If we have a vector of data (e.g. because we just created this data), then we would like to take :cpp:`Rng==std::vector`. On the other hand, if we happen to just reference data that what given to us, through a span for instance, then we would like to take :cpp:`Rng==std_e::span`. And if we were to use memory created with a special allocator (e.g. :cpp:`MPI_Alloc_mem`), then we would need still another range type.

The solution is then to take a class that type-erases any contiguous range. This is what is done by using a :cpp:`std_e::polymorphic_array`.

Conclusion
''''''''''

At the end, our :cpp:`node_value` is more or less a :cpp:`multi_array<variant_range<polymorphic_array>>`: a multi-dimensional array of dynamic rank, whose underlying range is type-erased both regarding its scalar coefficients type and its creation method.

This allows :cpp:`node_value` to be built from various types, and to offer a flexible, unified interface. Of course this flexibility comes at the cost of several levels of indirection that are not needed if the user knows the specific kind of array to be dealt with. That is why the type-erased interface must be used carefully, for example for small arrays (e.g. the arrys of `PointRange`, `ElementRange`, `Zone_t`, `Element_t`, which contain only 2 to 6 coefficients).

So in order to use arrays efficiently, the user must view the :cpp:`node_value` with a fixed rank and a static scalar type, see :ref:`view_with_type`. This is easy most of the time, because the SIDS enforces this parameters.


.. _view_with_type:

Viewing node values with static types and ranks
"""""""""""""""""""""""""""""""""""""""""""""""

The :cpp:`node_value` interface is flexible and can be used for small arrays. Moreover, having one single type for a node value is mandatory to built proper tree structure. However, for efficiency reasons, and also because it is often nice to use static typing, a :cpp:`node_value` can be viewed as a "regular" multi-dimensional array with fixed type and fixed rank.

* For mono-dimensional arrays:

.. literalinclude:: /../cpp_cgns/base/test/node_value_conversion.test.cpp
  :language: C++
  :start-after: [Sphinx Doc] node_value as span {
  :end-before: [Sphinx Doc] node_value as span }

* For multi-dimensional arrays:

.. literalinclude:: /../cpp_cgns/base/test/node_value_conversion.test.cpp
  :language: C++
  :start-after: [Sphinx Doc] node_value as md_array {
  :end-before: [Sphinx Doc] node_value as md_array }

* The :cpp:`view_as_array<T,rank=1>(node_value& x)` function can be used and will dispatch to either :cpp:`span` or :cpp:`md_array_view`:

.. literalinclude:: /../cpp_cgns/base/test/node_value_conversion.test.cpp
  :language: C++
  :start-after: [Sphinx Doc] node_value view_as_array {
  :end-before: [Sphinx Doc] node_value view_as_array }

* Convert to a string: works with any scalar type, but especially useful for :cpp:`C1`

.. literalinclude:: /../cpp_cgns/base/test/node_value_conversion.test.cpp
  :language: C++
  :start-after: [Sphinx Doc] node_value to_string {
  :end-before: [Sphinx Doc] node_value to_string }

SIDS
^^^^

TODO expand (Tree_range, tree_manip, sids/creation)
For and tree_manip, sids/creation: exhaustive (example of Cassiopee)

Tree manipulation (``tree_manip.hpp``)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Utility fonctions are provided to ease tree navigation. The main functions are:

.. code:: c++

  tree& get_child_by_name(tree& t, const std::string& name);
  tree& get_child_by_label(tree& t, const std::string& type);
  tree_range get_children_by_label(tree& t, const std::string& type);
  tree_range get_children_by_labels(tree& t, const std::vector<std::string>& types);

  tree& get_node_by_matching(tree& t, const std::string& gen_path);
  tree_range get_nodes_by_matching(tree& t, const std::string& gen_path);

If several nodes do match, functions :cpp:`get_child_by_label` and :cpp:`get_node_by_matching` return the first found.

A :cpp:`tree_range` is a :cpp:`std::vector` of nodes, i.e. a vector of :cpp:`cgns::tree` references (note: since a vector cannot hold built-in references, it is actually a vector of :cpp:`std::reference_wrapper<tree>`).

The :cpp:`gen_path` argument of the two last functions is a "generalized" path: a "/"-separated string, with each element being the name of a node (like a regular path) or the type of a node. Example:

.. code:: c++

  tree_range bcdata_nodes = get_nodes_by_matching(zone_node,"ZoneBC/BC_t/BCDataSet_t/BCData_t");
  // "ZoneBC" is the name of a node, and "BC_t", "BCDataSet_t", and "BCData_t" are labels of a node



.. _interoperability:

Interoperability
----------------

Python/CGNS
^^^^^^^^^^^

TODO update (here, outdated)

C++/CGNS trees are compatible with Python/CGNS trees. The library uses `PyBind11 <https://github.com/pybind/pybind11>`_ to give access to Python types on the C++ side.

* :cpp:`python/CGNS` -> :cpp:`cgns::tree`:

.. code:: c++

  tree view_as_cpptree(py::list py_tree);

The :cpp:`py_tree` argument is a regular python/CGNS tree, accessible in C++ through this :cpp:`py::list` type. The arrays in the resulting C++/CGNS tree are just views pointing to the Python/CGNS arrays and the memory is valid as long as the Python objects are alive.

* :cpp:`cgns::tree` -> :cpp:`python/CGNS`:

.. code:: c++

  py::list view_as_pytree(tree& t)
  py::list pytree_with_transfered_ownership(tree& t, cgns_allocator& alloc)
  void update_and_transfer_ownership2(tree& t, cgns_allocator& alloc, py::list pytree)


The memory is shared in all cases. With :cpp:`view_as_pytree`, there is no memory ownership transfer. This means the Python/CGNS tree is valid as long as the C++ objects remain alive. With :cpp:`pytree_with_transfered_ownership` and :cpp:`update_and_transfer_ownership2`, the memory ownership is transfered on the Python side (Note: this is a cheap operation since only the ownership is transfered, not the memory itself. Concretely, this mean that the Python garbage collector will deallocate the memory allocated from the C++ side).

