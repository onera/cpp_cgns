#include "std_e/unit_test/doctest.hpp"
#include "cpp_cgns/cgns.hpp"


using namespace cgns;


TEST_CASE("Hand-written tree construction") {
  std::vector<R8> values = {42.,43.,44};
  I8 data_size = values.size();

  tree t = {
    "MyArray", // name
    "DataArray_t", // SIDS label
    {"R8",{data_size},values.data()}, // value
    {} // children (none here)
  };

  CHECK( t.name == "MyArray" );

  CHECK( t.value.data_type == "R8" );
  CHECK( t.value.dims == std::vector<I8>{3} );
  CHECK( ((R8*)t.value.data)[0] == 42. );
  CHECK( ((R8*)t.value.data)[1] == 43. );
  CHECK( ((R8*)t.value.data)[2] == 44. );

  CHECK( t.children.size() == 0 );

  CHECK( t.label == "DataArray_t" );




  SUBCASE("Adding a sub-tree as a child") {
    std::vector<I4> sub_values = {2,3,4,5,6,7};
    tree sub_t = {
      "SubArray",
      "IndexArray_t",
      {"I4",{2,3},sub_values.data()}, // the user is responsible for giving 
                                      // the corrent data_type ("I4") 
                                      // and the correct dimension (here, 2 x 3)
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

  node_value x0 = {"I4",{5},v0.data()};
  node_value x1 = {"I4",{3},v0.data()};
  node_value x2 = {"R4",{5},v0.data()};
  node_value x3 = {"I4",{5},v1.data()};
  node_value x4 = {"I4",{5},v2.data()};

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
  node_value val0 = {"I4",{1}  ,v0.data()};
  node_value val1 = {"R8",{2,2},v1.data()};

  tree t0 = {
    "A", "A_t", val0, {
      tree{"B0", "B_t", MT, {
          tree{"D", "A_t", MT, {}} } },
      tree{"B1", "B_t", val0, {
          tree{"D", "D_t", MT, {}} } } }
  };

  // same
  tree t1 = {
    "A", "A_t", val0, {
      tree{"B0", "B_t", MT, {
          tree{"D", "A_t", MT, {}} } },
      tree{"B1", "B_t", val0, {
          tree{"D", "D_t", MT, {}} } } }
  };

  // changed value at the root
  tree t2 = {
    "A", "A_t", val1, {
      tree{"B0", "B_t", MT, {
          tree{"D", "A_t", MT, {}} } },
      tree{"B1", "B_t", val0, {
          tree{"D", "D_t", MT, {}} } } }
  };

  // changed value at the end
  tree t3 = {
    "A", "A_t", val0, {
      tree{"B0", "B_t", MT, {
          tree{"D", "A_t", MT, {}} } },
      tree{"B1", "B_t", val0, {
          tree{"D", "D_t", val1, {}} } } }
  };

  // removed node in the middle
  tree t4 = {
    "A", "A_t", val0, {
      tree{"B0", "B_t", MT, {} },
      tree{"B1", "B_t", val0, {
          tree{"D", "D_t", MT, {}} } } }
  };

  // removed node at the end
  tree t5 = {
    "A", "A_t", val0, {
      tree{"B0", "B_t", MT, {
          tree{"D", "A_t", MT, {}} } },
      tree{"B1", "B_t", val0, {} } }
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
