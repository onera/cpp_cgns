#include "std_e/unit_test/doctest.hpp"
#include "cpp_cgns/sids/creation.hpp"
#include "cpp_cgns/interop/pycgns_converter.hpp"
#include "std_e/log.hpp" // TODO
#include "pybind11/numpy.h"
#include <pybind11/embed.h> 


using namespace cgns;
namespace py = pybind11;


auto
conversion_cpp_test_case(cgns_allocator& alloc) -> tree {
  factory F(&alloc);
  tree b = F.newCGNSBase("Base",2,2);
    tree z0 = F.newUnstructuredZone("Z0",{5,3,0});
      auto elt_con_z0 = make_cgns_vector({1,2,3, 2,3,4, 3,4,5},F.alloc());
      emplace_child( z0 , F.newElements("tris", TRI_3, std_e::make_span(elt_con_z0), 1, 3) );

    tree z1 = F.newUnstructuredZone("Z1",{7,4,0});
      auto elt_con_z1 = make_cgns_vector({5,6,7, 1,2,3, 2,3,4, 3,4,5},F.alloc());
      emplace_child( z1 , F.newElements("tris", TRI_3, std_e::make_span(elt_con_z1), 1, 4) );

    emplace_child(b,std::move(z0));
    emplace_child(b,std::move(z1));

  return b;
}


auto
conversion_py_test_case() -> py::list {
  std::vector<int> base_dims = {2,2};

  std::vector<int> z0_dims = {5,3,0};
  std::vector<int> z0_elt_type = {TRI_3,0};
  std::vector<int> z0_elt_range_val = {1,3};
  std::vector<int> z0_elt_co_val = {1,2,3, 2,3,4, 3,4,5};

  std::vector<int> z1_dims = {7,4,0};
  std::vector<int> z1_elt_type = {TRI_3,0};
  std::vector<int> z1_elt_range_val = {1,4};
  std::vector<int> z1_elt_co_val = {5,6,7, 1,2,3, 2,3,4, 3,4,5};

  std::vector<int> zone_shape = {1,3};

  py::list b(4);
  b[0] = "Base";
  b[1] = py::array(py::dtype("i"),2,base_dims.data());
  b[2] = py::list();
  b[3] = "CGNSBase_t";


    py::list z0(4);
    z0[0] = "Z0";
    z0[1] = py::array(py::dtype("i"),zone_shape,z0_dims.data());
    z0[2] = py::list();
    z0[3] = "Zone_t";

      py::list z0_type(4);
      z0_type[0] = "ZoneType";
      z0_type[1] = py::array(py::dtype("b"),12,"Unstructured");
      z0_type[2] = py::list();
      z0_type[3] = "ZoneType_t";

      py::list z0_elt(4);
      z0_elt[0] = "tris";
      z0_elt[1] = py::array(py::dtype("i"),2,z0_elt_type.data());
      z0_elt[2] = py::list();
      z0_elt[3] = "Elements_t";

        py::list z0_elt_range(4);
        z0_elt_range[0] = "ElementRange";
        z0_elt_range[1] = py::array(py::dtype("i"),2,z0_elt_range_val.data());
        z0_elt_range[2] = py::list();
        z0_elt_range[3] = "IndexRange_t";

        py::list z0_elt_co(4);
        z0_elt_co[0] = "ElementConnectivity";
        z0_elt_co[1] = py::array(py::dtype("i"),z0_elt_co_val.size(),z0_elt_co_val.data());
        z0_elt_co[2] = py::list();
        z0_elt_co[3] = "DataArray_t";

        py::list z0_elt_children;
        z0_elt_children.append(std::move(z0_elt_range));
        z0_elt_children.append(std::move(z0_elt_co));
        z0_elt[2] = z0_elt_children;

      py::list z0_children;
      z0_children.append(std::move(z0_type));
      z0_children.append(std::move(z0_elt));
      z0[2] = z0_children; 


    py::list z1(4);
    z1[0] = "Z1";
    z1[1] = py::array(py::dtype("i"),zone_shape,z1_dims.data());
    z1[2] = py::list();
    z1[3] = "Zone_t";

      py::list z1_type(4);
      z1_type[0] = "ZoneType";
      z1_type[1] = py::array(py::dtype("b"),12,"Unstructured");
      z1_type[2] = py::list();
      z1_type[3] = "ZoneType_t";

      py::list z1_elt(4);
      z1_elt[0] = "tris";
      z1_elt[1] = py::array(py::dtype("i"),2,z1_elt_type.data());
      z1_elt[2] = py::list();
      z1_elt[3] = "Elements_t";

        py::list z1_elt_range(4);
        z1_elt_range[0] = "ElementRange";
        z1_elt_range[1] = py::array(py::dtype("i"),2,z1_elt_range_val.data());
        z1_elt_range[2] = py::list();
        z1_elt_range[3] = "IndexRange_t";

        py::list z1_elt_co(4);
        z1_elt_co[0] = "ElementConnectivity";
        z1_elt_co[1] = py::array(py::dtype("i"),z1_elt_co_val.size(),z1_elt_co_val.data());
        z1_elt_co[2] = py::list();
        z1_elt_co[3] = "DataArray_t";

        py::list z1_elt_children;
        z1_elt_children.append(std::move(z1_elt_range));
        z1_elt_children.append(std::move(z1_elt_co));
        z1_elt[2] = z1_elt_children;

      py::list z1_children;
      z1_children.append(std::move(z1_type));
      z1_children.append(std::move(z1_elt));
      z1[2] = z1_children;


    py::list b_children;
    b_children.append(std::move(z0));
    b_children.append(std::move(z1));
    b[2] = b_children;

  return b;
}



auto
conversion_cpp_test_case2(cgns_allocator& alloc) -> tree {
  factory F(&alloc);
  tree b = F.newCGNSBase("Base",2,2);
    tree z0 = F.newUnstructuredZone("Z0",{5,3,0});
      auto elt_con_z0 = make_cgns_vector({1,2,3, 2,3,4, 3,4,5},F.alloc());
      emplace_child( z0 , F.newElements("tris", TRI_3, std_e::make_span(elt_con_z0), 1, 3) );

    tree z1 = F.newUnstructuredZone("Z1",{7,4,0});
      auto elt_con_z1 = make_cgns_vector({5,6,7, 1,2,3, 2,3,4, 3,4,5},F.alloc());
      emplace_child( z1 , F.newElements("tris", TRI_3, std_e::make_span(elt_con_z1), 1, 4) );

    emplace_child(b,std::move(z0));
    emplace_child(b,std::move(z1));

  return b;
}


//TEST_CASE("view_as_cpptree") {
//  py::scoped_interpreter guard;
//  cgns_allocator alloc;
//
//  auto cpp_tree = conversion_cpp_test_case(alloc;
//  auto py_tree = conversion_py_test_case();
//
//  auto cpp_tree_from_py = view_as_cpptree(py_tree);
//  CHECK( cpp_tree_from_py == cpp_tree );
//}
//
//
//TEST_CASE("view_as_pytree") {
//  py::scoped_interpreter guard;
//  cgns_allocator alloc;
//
//  auto cpp_tree = conversion_cpp_test_case(alloc);
//  auto py_tree_from_cpp = view_as_pytree(cpp_tree);
//
//  CHECK( view_as_cpptree(py_tree_from_cpp) == cpp_tree );
//}


//TEST_CASE("pytree_with_transfered_ownership") {
//  py::scoped_interpreter guard;
//  py::list py_tree;
//
//  {
//    cgns_allocator alloc;
//    auto cpp_tree = conversion_cpp_test_case(alloc);
//    py_tree = pytree_with_transfered_ownership(cpp_tree,alloc);
//  } // At this point, alloc is destroyed, but the memory it was holding has been transfered to Python
//
//  cgns_allocator check_alloc;
//  auto expected_cpp_tree = conversion_cpp_test_case(check_alloc);
//  CHECK( view_as_cpptree(py_tree) == expected_cpp_tree);
//}

auto
my_test_operation(tree& t, cgns_allocator& alloc) {
  factory F(&alloc);
  emplace_child(t,F.newDataArray("MyData",{0,1,2}));
}

TEST_CASE("update_and_transfer_ownership") {
  py::scoped_interpreter guard;
 
  // Typical workflow :
  /// 0. Start with a tree in Python
  auto py_tree = conversion_py_test_case();
  { /// 1. Call a binary module (here represented by this opening scope)
    /// 2. Convert from a py_tree
    auto cpp_tree = view_as_cpptree(py_tree);
    /// 3. Permform operations that add nodes (hence, memory) to the C++ tree
    cgns_allocator alloc;
    my_test_operation(cpp_tree,alloc);
    /// 4. Update the py_tree and give it the ownership
    update_and_transfer_ownership2(cpp_tree,alloc,py_tree);
  } /// 5. Return to Python. At this point, alloc is destroyed, but the memory it was holding has been transfered to Python

  cgns_allocator check_alloc;
  auto expected_cpp_tree = conversion_cpp_test_case(check_alloc);
  my_test_operation(expected_cpp_tree,check_alloc);
  CHECK( view_as_cpptree(py_tree) == expected_cpp_tree );
}
