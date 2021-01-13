.. _user_manual:

###########
User Manual
###########

.. currentmodule:: cpp_cgns

.. _memory_management:

Memory management
^^^^^^^^^^^^^^^^^

A CGNS tree provides a hierarchical, standardized way of storing and exchanging data. Programs may want to interact over the same CFD problem. Hence they want to read and write the same tree, yet they have little knowledge of each other and they even want to minimize their relation with each other. In particular, they want to be able to manage their own part of the tree, and not the parts handled by the other programs: we can say that there is a **contract** (implicit or explicit) between them on how they interact over the CFD data (i.e. the tree). In particular each program wants to manage the memory of its parts of the tree, and not care too much about the implementation of the memory management on the other side. This is why memory management is not built-in the tree: the tree needs to be shared among the programs that interact with it, but the memory managament of each program should be kept private.

For managing an owned part of a :cpp:`cgns::tree` (i.e. allocating and deallocating its memory), the idea is to associate the tree with a :cpp:`cgns_allocator`, allocate memory with it, create nodes out of it, and then put them in the tree. Nodes can then be erased by first deallocating the memory through the allocator, then removing the node from the tree. The key point is that **the allocated memory lifetime is bound to the allocator**, and is ended when the allocator lifetime ends. The allocator should be viewed as **a handle to a owned part of the tree**: once it goes out of scope, so does the memory.

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

  void bad_use(tree& t) {
    cgns_allocator alloc;
    tree n = create_node(alloc);

    t.children.push_back(n); // Maybe fine, but beware of the shallow copy
    use_tree_with_new_node(t);
  } // BAD: alloc goes out of scope, the t.children.back() points to now invalid memory

  void good_use(tree& t) {
    cgns_allocator alloc;
    tree n = create_node(alloc);

    emplace_child(t,std::move(n)); // emplace_child forces to use a move
    use_tree_with_new_node(t);
    t.children.pop_back(); // remove node "n"
  } // OK: alloc goes out of scope, but the tree does not refer to reclaimed memory anymore

  cgns_allocator other_good_use(tree& t) {
    cgns_allocator alloc;
    tree n = create_node(alloc);

    emplace_child(t,std::move(n));
    use_tree_with_new_node(t);
    return alloc;
  } // OK: alloc still alive, so is its memory


For example, imagine a scenario where the file reading (i.e. building of the tree) is done in python, then the pre-processing in C++ reads the tree, give it to the solver, and then output the result to new nodes, and afterwards the tree is handled to a mesh deformation module which changes the coordinates (without reallocating memory). Then there are four programs interacting with the data: the file reader, the pre-processor, the solver and the mesh deformation module. In particular, the file reader and the pre-processor do not need to know each other's memory management. Since the file reader provides a python/CGNS tree, its memory is allocated through numpy array. Then when handled to the pre-processor, the tree is converted to :cpp:`cgns::tree` (see :ref:`interoperability`) and the C++ program has a tree with pointers to external memory. Then it can create a :cpp:`cgns_allocator` object, to allocate new nodes in the tree with it, and keep the allocator alive as long as the memory it allocated for the node is in use.

NOTES: 

* Memory ownership can be reclaimed from an allocator (e.g. to transfer the ownership to Python or to another allocator). For that, use the member function :cpp:`cgns_allocator::release_if_owner(void* ptr)` that will tell the allocator that it does not own :cpp:`ptr` anymore (and hence cannot deallocate it). Now that you have the memory, you need a function that can deallocate it and is matching the one of the allocator. You can ask such a function with member function :cpp:`cgns_allocator::memory_destructor_function()`.
* a :cpp:`cgns_allocator` is not a :cpp:`std` compliant allocator since it needs to allocate several memory types for the tree (e.g. :cpp:`I4`, :cpp:`R8`...). Its :cpp:`allocate` function is templated by the type to allocate, hence it does not match the :cpp:`std` allocator specification. A  :cpp:`std` compliant convenience class :cpp:`cgns_std_allocator` is provided that associates a :cpp:`cgns_allocator` with a type (e.g. :cpp:`I4` or :cpp:`R8`). It can be used e.g. with a :cpp:`cgns_vector`: an `std::vector` using this :cpp:`cgns_std_allocator` as its allocator.
* if a :cpp:`cgns_allocator` is asked to deallocate memory it does not own, it does nothing. This is the expected behavior: if a program does not hold the memory of a node, it isn't responsible for deallocating its memory.                                        
* DO NOT create two CGNS nodes pointing to the same memory. It is unsupported (e.g. deallocating data from one node will deallocate silently on the other node). But fundamentally the meaning is dubious: if two nodes share the same memory, why are there two nodes in the first place? Using the CGNS tree as a DAG is possible within the SIDS (e.g. through use of families) so it should not be done at the node memory level.
                                                                                                                                                                                                                                                               
