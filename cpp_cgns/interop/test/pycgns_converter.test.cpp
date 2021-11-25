#include "std_e/unit_test/doctest_pybind.hpp"

#include "cpp_cgns/interop/pycgns_converter.hpp"
#include "cpp_cgns/sids/creation.hpp"
#include "cpp_cgns/tree_manip.hpp"
#include "pybind11/numpy.h"


using namespace cgns;
namespace py = pybind11;


auto
cpp_tree_example() -> tree {
  tree b = new_CGNSBase("Base",2,2);
    tree z0 = new_UnstructuredZone("Z0",{5,3,0});
      std::vector elt_con_z0 = {1,2,3, 2,3,4, 3,4,5};
      emplace_child( z0 , new_Elements("tris", TRI_3, std::move(elt_con_z0), 1, 3) );

    tree z1 = new_UnstructuredZone("Z1",{7,4,0});
      std::vector elt_con_z1 = {5,6,7, 1,2,3, 2,3,4, 3,4,5};
      emplace_child( z1 ,new_Elements("tris", TRI_3, std::move(elt_con_z1), 1, 4) );

    emplace_child(b,std::move(z0));
    emplace_child(b,std::move(z1));

  return b;
}


auto
py_tree_example() -> py::list {
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
      z0_type[1] = py::array(py::dtype("|S1"),12,"Unstructured");
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
      z1_type[1] = py::str("Unstructured");
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




PYBIND_TEST_CASE("view_as_cpptree") {
  auto cpp_tree = cpp_tree_example();
  auto py_tree = py_tree_example();

  auto cpp_tree_from_py = to_cpp_tree(py_tree);
  CHECK( cpp_tree_from_py == cpp_tree );
}


PYBIND_TEST_CASE("view_as_pytree") {
  auto cpp_tree = cpp_tree_example();
  auto py_tree_from_cpp = to_py_tree(cpp_tree);

  CHECK( to_cpp_tree(py_tree_from_cpp) == cpp_tree );
}


PYBIND_TEST_CASE("pytree_with_transfered_ownership") {
  py::list py_tree;
  {
    auto cpp_tree = cpp_tree_example();
    py_tree = to_owning_py_tree(std::move(cpp_tree));
  } // At this point, cpp_tree is destroyed, but the memory ownership has been transfered to Python


  auto expected_cpp_tree = cpp_tree_example();
  CHECK( to_cpp_tree(py_tree) == expected_cpp_tree);
}

auto
my_test_operation(tree& t) {
  emplace_child(t,new_DataArray("MyData",std::vector{0,1,2}));
  tree& z0 = get_child_by_name(t,"Z0");
  rm_child_by_name(z0,"ZoneType");
}

PYBIND_TEST_CASE("update_and_transfer_ownership") {
  // Typical workflow :
  /// 0. Start with a tree in Python
  auto py_tree = py_tree_example();
  { /// 1. Call a binary module (here represented by this opening scope)
    /// 2. Convert from a py_tree
    auto cpp_tree = to_cpp_tree(py_tree);
    /// 3. Permform operations that add nodes (hence, memory) to the C++ tree
    my_test_operation(cpp_tree);
    /// 4. Update the py_tree and give it the ownership
    update_and_transfer_ownership_to_py_tree(std::move(cpp_tree),py_tree);
  } /// 5. Return to Python. At this point, C++ node_value objects are destroyed, but the memory they where holding has been transfered to Python

  auto expected_cpp_tree = cpp_tree_example();
  my_test_operation(expected_cpp_tree);
  CHECK( to_cpp_tree(py_tree) == expected_cpp_tree );
}
