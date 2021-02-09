#include "std_e/unit_test/doctest.hpp"
#include "cpp_cgns/cgns.hpp"
#include "cpp_cgns/node_manip.hpp"
#include <memory>


using namespace cgns;


TEST_CASE("Hand-written tree construction") {
  tree t = {
    "MyArray", // name
    "DataArray_t", // SIDS label
    create_node_value({42.,43.,44.}), // value (here, the default are used: the value will be hold by a buffer using malloc/free)
    {} // children (none here)
  };

  CHECK( t.name == "MyArray" );

  CHECK( t.value.data_type == "R8" );
  CHECK( t.value.dims == std::vector<I8>{3} );
  CHECK( ((R8*)data(t.value))[0] == 42. );
  CHECK( ((R8*)data(t.value))[1] == 43. );
  CHECK( ((R8*)data(t.value))[2] == 44. );

  CHECK( t.children.size() == 0 );

  CHECK( t.label == "DataArray_t" );




  SUBCASE("Adding a sub-tree as a child") {
    md_array<I4,2> sub_values = {{2,3,4},{5,6,7}};
    tree sub_t = {
      "SubArray",
      "IndexArray_t",
      make_node_value(std::move(sub_values)),
      {}
    };
    emplace_child(t,std::move(sub_t));

    CHECK( t.children.size() == 1 );
    CHECK( t.children[0].name == "SubArray" );
  }
}




TEST_CASE("same_node_data and equal_node_data") {
  std::vector<int> v0 = {10,3,4,1,5};
  std::vector<int> v1 = {10,3,4,1,5};
  std::vector<int> v2 = {10,3,4,1,15};

  node_value x0 = {"I4",{5},std_e::buffer_view(v0)};
  node_value x1 = {"I4",{3},std_e::buffer_view(v0)};
  node_value x2 = {"R4",{5},std_e::buffer_view(v0)};
  node_value x3 = {"I4",{5},std_e::buffer_view(v1)};
  node_value x4 = {"I4",{5},std_e::buffer_view(v2)};

  CHECK(  same_node_data(x0,x0) );
  CHECK( !same_node_data(x0,x1) );
  CHECK( !same_node_data(x0,x2) );
  CHECK( !same_node_data(x0,x3) );
  CHECK( !same_node_data(x0,x4) );

  CHECK( x0==x0 );
  CHECK( x0!=x1 );
  CHECK( x0!=x2 );
  CHECK( x0==x3 );
  CHECK( x0!=x4 );
}




TEST_CASE("tree equality") {
  std::vector<I4> v0 ={0,1};
  std::vector<R8> v1 ={2.,3.,4.,5.};
  auto val0 = create_node_value({0,1});
  auto val1 = create_node_value({2.,3.,4.,5.});

  tree t0 = {
    "A", "A_t", view(val0), {
      tree{"B0", "B_t", MT(), {
          tree{"D", "A_t", MT(), {}} } },
      tree{"B1", "B_t", view(val0), {
          tree{"D", "D_t", MT(), {}} } } }
  };

  // same
  tree t1 = {
    "A", "A_t", view(val0), {
      tree{"B0", "B_t", MT(), {
          tree{"D", "A_t", MT(), {}} } },
      tree{"B1", "B_t", view(val0), {
          tree{"D", "D_t", MT(), {}} } } }
  };

  // changed value at the root
  tree t2 = {
    "A", "A_t", view(val1), {
      tree{"B0", "B_t", MT(), {
          tree{"D", "A_t", MT(), {}} } },
      tree{"B1", "B_t", view(val0), {
          tree{"D", "D_t", MT(), {}} } } }
  };

  // changed value at the end
  tree t3 = {
    "A", "A_t", view(val0), {
      tree{"B0", "B_t", MT(), {
          tree{"D", "A_t", MT(), {}} } },
      tree{"B1", "B_t", view(val0), {
          tree{"D", "D_t", view(val1), {}} } } }
  };

  // removed node in the middle
  tree t4 = {
    "A", "A_t", view(val0), {
      tree{"B0", "B_t", MT(), {} },
      tree{"B1", "B_t", view(val0), {
          tree{"D", "D_t", MT(), {}} } } }
  };

  // removed node at the end
  tree t5 = {
    "A", "A_t", view(val0), {
      tree{"B0", "B_t", MT(), {
          tree{"D", "A_t", MT(), {}} } },
      tree{"B1", "B_t", view(val0), {} } }
  };

  CHECK( t0 == t0 );
  CHECK( t0 == t1 );
  CHECK( t0 != t2 );
  CHECK( t0 != t3 );
  CHECK( t0 != t4 );
  CHECK( t0 != t5 );

  CHECK(  same_tree_structure(t0,t0) );
  CHECK(  same_tree_structure(t0,t1) );
  CHECK(  same_tree_structure(t0,t2) );
  CHECK(  same_tree_structure(t0,t3) );
  CHECK( !same_tree_structure(t0,t4) );
  CHECK( !same_tree_structure(t0,t5) );
}
