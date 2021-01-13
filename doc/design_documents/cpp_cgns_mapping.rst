The C++/CGNS mapping
====================

Design philosophy
-----------------

The :ref:`cpp_cgns_mapping` data representation was chosen based on the the following principles:

* access data in a uniform, regular way
* external management of memory
* simple, lightweight data structure

More specifically,
* There are two kind of representations regarding CGNS trees. One would be to map each data structure described in the SIDS to a corresponding :cpp:`struct` e.g.

.. code:: c++

  struct CGNSBase_t {
    std::vector<Family_t> family;
    std::vector<Zone_t> zones;
  };

This way, the type safety ensures that the semantics of the SIDS constructs are bound to C++ objects. However, we find this approach inconvenient in practice. First, it forces people to agree on the data mapping of all SIDS structures; second, it is impossible to execute tree-wide operation when we do not care about the SIDS (write to a file, serialization...); third, the type system is actually not sufficient to ensure SIDS semantics. 

On the contrary, the approach here is to use a uniform tree without SIDS semantics. This mapping is very compact and regular. We think that semantics can be checked otherwise (check functions, preconditions...).

Discussion of the design choices
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* The :cpp:`node_value` data structure only holds a pointer to some memory. The user has to manage it by himself, and we think it is very important that the memory management facilies be separate from the tree itself. In a typical workflow, pointers to data can be managed by different entities, e.g. some numpy array there, some pointers to memory from a kernel solver... The tree only structures how data is organized, not where it comes from.

* The :cpp:`tree` structure is a compromise between simplicity, interoperability and convenience.
    * The :cpp:`node_value` data structure holds a :cpp:`void` pointer. Regarding CGNS, data values can be of type :cpp:`C1 (char)`, :cpp:`I4 (int32)`, :cpp:`I8`, :cpp:`R4 (float)`, :cpp:`R8`. We refrained ourselves from using :cpp:`union` or :cpp:`std::variant` which would not bring more type safety, but would certainly bring more complexity.
    * A node value is a list of dimensions and a pointer to memory, not directly a multi-dimensionnal array class. Indeed, using such a class would be too complex without any further knowledge of the node: what will be the type of the data? should the number of dimensions be fixed? does this class manages the memory? [Note: Python/CGNS has not the first two problems because of dynamic typing]. Note that conversion between :cpp:`node_value` and multidimensional arrays is provided through external functions.
    * We use :cpp:`std::string` and :cpp:`std::vector` for simplicity and convenience. Regarding interoperability, it could be a problem if we want to exchange with C or Fortran users, but we feel like not using these two basic classes would be to much of a burden.

* Use cases where the tree is not "a simple tree in memory" can't be addressed easily with this approach:
    * handling of non-contiguous memory arrays
    * lazy loading of trees

    Basically, the fact that we are not using opaque objects, but directly a pointer to memory, prevents from using any kind of proxy object within the representation of the tree.


As far as interoperability is concerned, the user is free to create and handle trees as he sees fit, but the **cpp_cgns** library proposes a set of facilities regarding memory management, tree construction, multi-dimensional arrays management, and interoperabily described in :ref:`tree_manip`.

Acknowledgements
----------------

The design of the C++/CGNS mapping is based on `Python/CGNS <https://github.com/pyCGNS/pyCGNS>`_. Tree manipulation functions are based on `Cassiopee <http://elsa.onera.fr/Cassiopee/>`_.
