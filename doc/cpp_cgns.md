## Introduction ##
`cpp_cgns` offers a mapping of CGNS trees into a C++ in-memory representation, plus utilities for manipulation of C++ CGNS trees.

The following documentation supposes basic knowledge of CGNS (node structure and SIDS). For a more technical documentation, see the unit tests.

## Data representation ##

A C++ CGNS tree is defined by the following recursive structure (file `cpp_cgns.h`):
```c++
struct tree {
  std::string name;
  node_value value;
  std::vector<tree> children;
  std::string type;
};
```

Where the value of a tree node is defined by the following data structure:
```c++
struct node_value {
  std::string data_type;
  std::vector<I8> dims;
  void* data;
};
```


## Tree manipulation ##
The `cpp_cgns::tree` structure is very low-level so that each node in the tree has the same uniform structure. It does not provide by itself type-safe access, multi-dimensional array access, memory management, nor SIDS manipulation facilities. However, these functionnalities are provided non-intrusively.

### Node values ###
`node_value` objects need not be handled directly in most cases. Conversions functions towards more usable data structures are provided in file `cpp_cgns_utils.h":
 * Conversion to md_array_view (non-owning multi-dimensional arrays):
```c++
  node_value val = /* ... */;
  md_array_view<I4,2> x = view_as_md_array<I4,2>(val); 
  // "x" is now a two-dimensional array of I4s pointing to the same memory as node_value "val"
  std::cout << "dimensions of x: " << x.extent() << "\n";
  std::cout << "value (0,1) of x: " << x(0,1) << "\n";
```
 * Conversion to span:
```c++
  node_value val = /* ... */;
  std_e::span<I4> x = view_as_span<I4>(val); 
  std::cout << "size of x: " << x.size() << "\n";
  // ...
```

There is no memory copy involved, the created objects point to the same memory the `node_value` object does.

In both cases the `value_type` of the `md_array_view`, or `span`, must be given at compile time. In general, the user knows the `data_type` from the SIDS. For instance, node connectivities are required to be of the I4 type; field value are R8... If the type does not match the string value of the `data_type` attribute, an exception/assertion is raised.

In case of `view_as_md_array`, an array rank must also be given at compile time. If unknown, the value `cpp_cgns::dyn_rank` should be given.

Conversion the other way around are also possible:
```c++
  md_array_view<I4,2> arr_2D = /* ... */;
  node_value v0 = view_as_node_value(arr_2D);

  span<I4> arr_1D = /* ... */;
  node_value v1 = view_as_node_value(arr_1D);
```

Here again, there is no copy.


### Memory management ###
A CGNS tree provides a hierarchical, standardized way of storing and exchanging data. Programs may want to interact over the same CFD problem. Hence they want to read and write the same tree, yet they have little knowledge of each other and they want to minimize their relation with each other. In particular, they want to be able to manage their own part of the tree, and not the parts handled by the other programs: we can say that there is a contract (implicit or explicit) between them on how they interact over the CFD data (i.e. the tree). In particular each program wants to manage the memory of its parts of the tree, and not care too much about the implementation of the memory management on the other side. This is why memory management is not built-in the tree: the tree needs to be shared among the programs that interact with it, but the memory managament of each program should be kept private.

For managing an owned part of a `cpp_cgns::tree` (i.e. allocating and deallocating its memory), the idea is to associate the tree with a `cgns_allocator`, allocate memory with it, create nodes out of it, and then put them in the tree. Nodes can then be erased by first deallocating the memory through the allocator, then removing the node from the tree. The key point is that the allocated memory lifetime is bound to the allocator, and is ended when the allocator lifetime ends. The allocator should be viewed as *a handle to a owned part of the tree*: once it goes out of scope, so does the memory.
```c++
tree create_node(cgns_allocator& alloc) {
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

  t.children.push_back(n); // Maybe fine, but beware of the shallow copy (or use std::move)
  use_tree_with_new_node(t);
} // BAD: alloc goes out of scope, the t.children.back() points to now invalid memory

void good_use(tree& t) {
  cgns_allocator alloc;
  tree n = create_node(alloc);

  emplace_child(t,std::move(n)); // emplace_child forces to use a move
  use_tree_with_new_node(t);
  t.children.pop_back(); // remove node "n"
} // OK: alloc goes out of scope, but the tree does not point to reclaimed memory anymore

cgns_allocator other_good_use(tree& t) {
  cgns_allocator alloc;
  tree n = create_node(alloc);

  emplace_child(t,std::move(n));
  use_tree_with_new_node(t);
  return alloc;
} // OK: alloc still alive, so is its memory

}
```

For example, imagine a scenario where the file reading (i.e. building of the tree) is done in python, then the pre-processing in C++ reads the tree, give it to the solver, and then output the result to new nodes, and afterwards the tree is handled to a mesh deformation module which changes the coordinates (without reallocating memory). Then there are four programs interacting with the data: the file reader, the pre-processor, the solver and the mesh deformation module. In particular, the file reader and the pre-processor do not need to know each other's memory management. Since the file reader provides a python/CGNS tree, its memory is allocated through numpy array. Then when handled to the pre-processor, the tree is converted to `cpp_cgns::tree` (see ## Interoperability ##) and the C++ program has a tree with pointers to external memory. Then it can create a `cgns_allocator` object, to allocate new nodes in the tree with it, and keep the allocator alive as long as the memory it allocated for the node is in use.

NOTES: 
 * there is no mechanism for memory ownership transfers. We don't have any use case for now. It should be easily possible for C/C++ programs, and also possible from/to python.
 * a `cgns_allocator` is not a `std` compliant allocator since it needs to allocate several memory types for the tree (e.g. I4, R8...). Its `allocate` function is templated by the type to allocate, hence it does not match the `std` allocator specification. A  `std` compliant convenience class `cgns_std_allocator` is provided that associates a `cgns_allocator` with a type (e.g. I4 or R8). It can be used e.g. with a `cgns_vector`: an `std::vector` using this `cgns_std_allocator` as its allocator.
 * if a `cgns_allocator` is ask to deallocate memory it does not own, it does nothing. This is the expected behavior: if a program does not hold the memory of a node, it isn't responsible for deallocating its memory.
 * DO NOT create two CGNS nodes pointing to the same memory. It is unsupported (e.g. deallocating data from one node will deallocate silently on the other node). But fundamentally the meaning is dubious: if two nodes share the same memory, why are there two nodes in the first place? Using the CGNS tree as a DAG is possible within the SIDS, e.g. through use of families.


### SIDS ###
Although it is possible to navigate the tree by hand and use a `cgns_allocator` directly to create nodes, tree manipulation and creation facilities are provided.

#### Tree manipulation (`sids_manip.h`) ####
The main functions are:
```c++
tree& get_child_by_name(tree& t, const std::string& name);
tree& get_child_by_type(tree& t, const std::string& type);
tree_range get_children_by_type(tree& t, const std::string& type);
tree_range get_children_by_types(tree& t, const std::vector<std::string>& types);

tree& get_node_by_matching(tree& t, const std::string& gen_path);
tree_range get_nodes_by_matching(tree& t, const std::string& gen_path);
```

If several nodes do match, functions `get_child_by_type` and `get_node_by_matching` return the first found.

A `tree_range` is a `std::vector` of nodes, i.e. a vector of `cpp_cgns::tree` references (note: since a vector cannot hold built-in references, it is actually a vector of `std::reference_wrapper<tree>`).

The `gen_path` argument of the two last functions is a "generalized" path: a "/"-separated string, with each element being the name of a node (like a regular path) or the type of a node. Example:
```c++
  tree_range bcdata_nodes = get_nodes_by_matching(zone_node,"ZoneBC/BC_t/BCDataSet_t/BCData_t");
  // "ZoneBC" is the name of a node, and "BC_t", "BCDataSet_t", and "BCData_t" are labels of a node
```

#### Node creation and removal (`sids_creation.h`) ####
Since node creation and removal need to handle memory, a `cgns_allocator` must be used. In order to use it more transparently, class `Internal` takes a pointer to a `cgns_allocator` and is used for creating and deleting nodes. Example:
```c++
create_and_manip_tree(cgns_allocator& alloc) {
  Internal I(&alloc);

  // creation
  tree base = I.newCGNSBase("Base_0",3,3));
  emplace_child(base, I.newUnstructuredZone("Zone_0",10000,6075));

  emplace_child(t, std::move(base));
  
  // use
  use_tree(t)

  // removing
  I.remove_child_by_type(t,"CGNSBase_t"); // erase tree from "Base_0", and deallocate its memory recursively
}
```

#### Utility functions ####
Small utility functions are provided in `sids_utils.h` and `elements_utils.h`.


## Interopeability ##

### Python/CGNS ###
`cpp_cgns` trees are completely compatible with python/CGNS trees. Functions `view_as_pytree` and `view_as_cpptree` from file `pycgns_converter.h` can be called on the C++ side to convert from/to python/CGNS trees.

 * `cpp_cgns::tree -> python/CGNS`:
```c++
  PyObject* view_as_pytree(tree& t)
```
The returned PyObject is a regular python/CGNS tree. Its numpy arrays do not allocate memory, they just point to the same memory as the cpp_cgns::tree.

 * `python/CGNS -> cpp_cgns::tree`:
```c++
tree view_as_cpptree(PyObject* pytree);
```

The memory is shared in both cases. If the user wants a separate copy of the tree, function `deep_copy` from file `cpp_cgns_utils.h" can be used.

### C and Fortran [EXPERIMENTAL] ###
`cpp_cgns::tree` objects can be handled in C via the `cgns_tree` opaque structure and its access functions in file `c_interop.h`.


## Design philosophy ##
This data representation was chosen based on the the following principles:
  * access data in a uniform, regular way
  * external management of memory
  * simple, lightweight data structure

More specifically,
  * There are two kind of representations regarding CGNS trees. One would be to map each data structure described in the SIDS to a corresponding `struct` e.g.
```c++
struct CGNSBase_t {
  std::vector<Family_t> family;
  std::vector<Zone_t> zones;
}
```
This way, the type safety ensures that the semantics of the SIDS constructs are bound to C++ objects. However, we find this approach inconvenient in practice. First, it forces people to agree on the data mapping of all SIDS structures; second, it is impossible to execute tree-wide operation when we do not care about the SIDS (write to a file, serialization...); third, the type system is actually not sufficient to ensure SIDS semantics. 
On the contrary, the approach here is to use a uniform tree without SIDS semantics. This mapping is very compact and regular. We think that semantics can be checked otherwise (check functions, preconditions...).

  * The `node_value` data structure only holds a pointer to some memory. The user has to manage it by himself, and we think it is very important that the memory management facilies be separate from the tree itself. In a typical workflow, pointers to data can be managed by different entities, e.g. some numpy array there, some pointers to memory from a kernel solver... The tree only structures how data is sorted, not where it comes from.

  * The tree structure is a compromise between simplicity, interoperability and convenience.
      * The `node_value` data structure holds a `void` pointer. Regarding CGNS, data values can be of type C1 (char), I4 (int32), I8, R4 (float), R8. We refrained ourselves from using `union` or `std::variant` which would not bring more type safety, but would certainly bring more complexity.
      * A node value is a list of dimensions and a pointer to memory, not directly a multi-dimensionnal array class. Indeed, using such a class would be too complex without any further knowledge of the node: what will be the type of the data? should the number of dimensions be fixed? does this class manages the memory? [Note: python/CGNS has not the first two problems because of dynamic typing]. Note that conversion between `node_value` and multidimensional arrays is provided through external functions.
      * We use `std::string` and `std::vector` for simplicity and convenience. Regarding interoperability, it could be a problem if we want to exchange with C or Fortran users, but we feel like not using these two basic classes would be to much of a burden.

  * Use cases where the tree is not "a simple tree in memory" can't be addressed easily with this approach.
      - handling of non-contiguous memory arrays
      - lazy loading of trees

    Basically, the fact that we are not using opaque objects, but directly a pointer to memory, prevents from using any kind of proxy object within the representation of the tree.


As far as interoperability is concerned, the user is free to create and handle trees as he sees fit, but the library proposes a set of facilities regarding memory management, tree construction, multi-dimensional arrays management, and interoperabily described in the previous sections.

## Dependencies ##
`cpp_cgns` is a C++14 library. It depends on `std_e`, which is header-only (provides generic algorithms and multi-dimensional array facilities). Python interoperability requires headers from Python and Numpy which are grouped in the `python_c_api` folder.

## Acknowledgements ##
The design of the C++/CGNS mapping is based on python/CGNS. Tree manipulation functions are based on Cassiopee.
