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

Below is a succinct presentation of the main facilities of the library. For a more detailed presentation, see the :ref:`user_manual`.

Everything is defined within the :cpp:`cgns` namespace.

In the examples below, when there is no ambiguity, we will assume that we are using the :cpp:`std` and :cpp:`cgns` namespaces:

.. code:: c++

  using namespace std;
  using namespace cgns;

.. .. sectnum::

.. _cpp_cgns_mapping:

C++/CGNS mapping
----------------


A C++ CGNS tree is defined by the following recursive structure (file ``cpp_cgns.hpp``):

.. code:: c++

  struct tree {
    std::string name;
    std::string label;
    node_value value;
    std::vector<tree> children;
  };

Where the value of a tree node is defined by the following data structure:

.. code:: c++

  struct node_value {
    std::string data_type;
    std::vector<I8> dims;
    void* data;
  };

.. _tree_manip:

Tree manipulation
-----------------

The :cpp:`cgns::node_value` structure is very low-level so that each node in the tree has the same uniform structure. It does not provide by itself type-safe access, multi-dimensional array access, memory management, nor SIDS manipulation facilities. However, these functionnalities are provided non-intrusively by the library.

Node values
^^^^^^^^^^^

:cpp:`node_value` objects do not need to be handled directly in most cases. Conversions functions towards more usable data structures are provided in file ``node_manip.hpp``:

* Conversion to :cpp:`md_array_view` (non-owning multi-dimensional arrays):

.. code:: c++

  node_value val = /* ... */;
  md_array_view<I4,2> x = view_as_md_array<I4,2>(val); 
  // "x" is now a two-dimensional array of I4s pointing to the same memory as node_value "val"
  std::cout << "dimensions of x: " << x.extent() << "\n";
  std::cout << "value (0,1) of x: " << x(0,1) << "\n";

* Conversion to :cpp:`span`:

.. code:: c++

  node_value val = /* ... */;
  std_e::span<I4> x = view_as_span<I4>(val); 
  std::cout << "size of x: " << x.size() << "\n";

There is no memory copy involved, the created object point to the same memory the :cpp:`node_value` object does.

In both cases, the :cpp:`value_type` of :cpp:`md_array_view` or :cpp:`span` (here `I4`) must be given at compile time. In general, the user knows the :cpp:`data_type` from the SIDS. For instance, node connectivities are required to be of the :cpp:`I4` or :cpp:`I8` type; field values are :cpp:`R4` or :cpp:`R8`... If the type does not match the string value of the :cpp:`data_type` attribute, an exception/assertion is raised.

In case of :cpp:`view_as_md_array`, an array rank must also be given at compile time. If unknown, the value :cpp:`cgns::dyn_rank` should be given.

Conversion the other way around are also possible:

.. code:: c++

  md_array_view<I4,2> arr_2D = /* ... */;
  node_value v0 = view_as_node_value(arr_2D);

  span<I4> arr_1D = /* ... */;
  node_value v1 = view_as_node_value(arr_1D);

Here again, there is no copy.

Creating new nodes - Memory management
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Because we don't want to impose a specific way to allocate memory as part of the C++/CGNS mapping, no memory allocation feature is bound to the :cpp:`cgns::tree`. While memory management could be done by hand through :cpp:`malloc/free` or :cpp:`new/delete`, a more scalable possibility is to use a :cpp:`cgns_allocator`. The idea is to bound the owning of the memory to the allocator: each time memory is needed for a node, the memory is asked to the allocator through the :cpp:`allocate` function:

.. code:: c++

  tree create_my_node_node(cgns_allocator& alloc) {
    int sz = 3;
    I4* mem = allocate<I4>(alloc,sz);
    mem[0] = 42;
    mem[1] = 43;
    mem[2] = 44;

    return {
      "my_node",
      {"I4",{sz},mem},
      {}, // no children
      "UserDefinedData_t"
    }
  }

**The memory is owned by the** :cpp:`cgns_allocator`. It means that the :cpp:`cgns_allocator` object **has to be kept alive as long as its memory is used** by a :cpp:`cgns::tree`.

The interest of this strategy comes from the fact that it is not exclusive: there can be several allocators associated to several life durations of nodes of the tree; and it can be combined with other strategies, e.g. the external allocation of Python/CGNS tree if the tree is indeed also used on the Python side. Memory management is detailed :ref:`here <memory_management>`.


SIDS
^^^^

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

Node creation and removal (``sids/creation.hpp``)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Although it is possible to use a :cpp:`cgns_allocator` directly to create nodes, SIDS creation facilities are provided. The :cpp:`cgns_allocator` is still the owner of the allocated memory, however, it delegates node creation/removal to a :cpp:`cgns::factory`.

.. code:: c++

  create_and_manip_tree(cgns_allocator& alloc) {
    cgns::factory F(&alloc); // create the factory. The factory *does not* own the allocator,
                             // it will just ask it to create memory

    // creation
    tree base = F.newCGNSBase("Base_0",3,3));
    emplace_child(base, F.newUnstructuredZone("Zone_0",10000,6075));

    emplace_child(t, std::move(base));
    
    // use
    use_tree(t)

    // removing
    F.remove_child_by_label(t,"CGNSBase_t"); // erase tree from "Base_0", and deallocate its memory recursively
  }


.. _interoperability:

Interoperability
----------------

Python/CGNS
^^^^^^^^^^^

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

