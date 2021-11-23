#include "std_e/unit_test/doctest.hpp"
#include "cpp_cgns/cgns.hpp"


using namespace cgns;


TEST_CASE("Hand-written tree construction") {
  tree t = {
    "MyArray", // name
    "DataArray_t", // SIDS label
    node_value({42.,43.,44.}), // value (here, the value will be hold by a std::vector)
    {} // children (none here)
  };

  CHECK( t.name == "MyArray" );

  CHECK( t.value.data_type() == "R8" );
  CHECK( t.value.rank() == 1 );
  CHECK( t.value.extent(0) == 3 );
  CHECK( t.value(0) == 42. );
  CHECK( t.value(1) == 43. );
  CHECK( t.value(2) == 44. );

  CHECK( t.children.size() == 0 );

  CHECK( t.label == "DataArray_t" );




  SUBCASE("Adding a sub-tree as a child") {
    md_array<I4,2> sub_values = {{2,3,4},{5,6,7}};
    tree sub_t = {
      "SubArray",
      "IndexArray_t",
      node_value(std::move(sub_values)),
      {}
    };
    emplace_child(t,std::move(sub_t));

    CHECK( t.children.size() == 1 );
    CHECK( t.children[0].name == "SubArray" );
  }
}




TEST_CASE("tree equality") {
  std::vector<I4> v0 = {0,1};
  std::vector<R8> v1 = {2.,3.,4.,5.};
  auto val0 = std_e::make_span(v0);
  auto val1 = std_e::make_span(v1);

  tree t0 = {
    "A", "A_t", node_value(val0), {
      tree{"B0", "B_t", MT(), {
          tree{"D", "A_t", MT(), {}} } },
      tree{"B1", "B_t", node_value(val0), {
          tree{"D", "D_t", MT(), {}} } } }
  };

  // same
  tree t1 = {
    "A", "A_t", node_value(val0), {
      tree{"B0", "B_t", MT(), {
          tree{"D", "A_t", MT(), {}} } },
      tree{"B1", "B_t", node_value(val0), {
          tree{"D", "D_t", MT(), {}} } } }
  };

  // changed value at the root
  tree t2 = {
    "A", "A_t", node_value(val1), {
      tree{"B0", "B_t", MT(), {
          tree{"D", "A_t", MT(), {}} } },
      tree{"B1", "B_t", node_value(val0), {
          tree{"D", "D_t", MT(), {}} } } }
  };

  // changed value at the end
  tree t3 = {
    "A", "A_t", node_value(val0), {
      tree{"B0", "B_t", MT(), {
          tree{"D", "A_t", MT(), {}} } },
      tree{"B1", "B_t", node_value(val0), {
          tree{"D", "D_t", node_value(val1), {}} } } }
  };

  // removed node in the middle
  tree t4 = {
    "A", "A_t", node_value(val0), {
      tree{"B0", "B_t", MT(), {} },
      tree{"B1", "B_t", node_value(val0), {
          tree{"D", "D_t", MT(), {}} } } }
  };

  // removed node at the end
  tree t5 = {
    "A", "A_t", node_value(val0), {
      tree{"B0", "B_t", MT(), {
          tree{"D", "A_t", MT(), {}} } },
      tree{"B1", "B_t", node_value(val0), {} } }
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
