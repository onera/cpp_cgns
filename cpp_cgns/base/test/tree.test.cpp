#include "std_e/unit_test/doctest.hpp"
#include "cpp_cgns/cgns.hpp"

using namespace cgns;

TEST_CASE("Hand-written tree construction - base") {
  // [Sphinx Doc] cgns::tree first example {
  tree t = {
    "Base", // node name
    "CGNSBase_t", // SIDS label
    node_value({3,3}), // 1D array of 3 I4
    {
      tree{"Z0","Zone_t",node_value({8 ,1,0})}, // first child
      tree{"Z1","Zone_t",node_value({12,2,0})}  // second child
    }
  };

  CHECK( name(t) == "Base" ); // name(t) returns a `std::string&`
  CHECK( label(t) == "CGNSBase_t" ); // label(t) returns a `std::string&`

  // `value(t)` returns a `cgns::node_value`, which is a multi-dimensional array
  CHECK( value(t).data_type() == "I4" );
  CHECK( value(t).rank() == 1 );
  CHECK( value(t).extent(0) == 2 );
  // since `value(t).rank()==1`, the array can be compared with a mono-dimensional array
  CHECK( value(t) == std::vector{3,3} );

  // tree `t` has two children which are of type `cgns::tree` themselves
  CHECK( number_of_children(t) == 2 );

  // children can be accessed one by one through the `child(t,i)` function
  CHECK( name(child(t,0)) == "Z0" );
  CHECK( name(child(t,1)) == "Z1" );

  // the `children(t)` function can also be used
  for (const tree& c : children(t)) {
    CHECK( label(c) == "Zone_t" );
  }
  // [Sphinx Doc] cgns::tree first example }
}

TEST_CASE("Adding a sub-tree as a child") {
  // [Sphinx Doc] adding a child {
  tree t = {
    "MyNode",
    "DataArray_t",
    node_value({42.,43.,44.})
  };

  tree sub_t = {
    "SubNode",
    "IndexArray_t",
    node_value({{2,3,4},{5,6,7}})
  };

  emplace_child(t,std::move(sub_t));

  CHECK( number_of_children(t) == 1 );
  CHECK( name(child(t,0)) == "SubNode" );
  // [Sphinx Doc] adding a child }
}




TEST_CASE("tree equality") {
  // [Sphinx Doc] tree equality {
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
  // [Sphinx Doc] tree equality }
}
