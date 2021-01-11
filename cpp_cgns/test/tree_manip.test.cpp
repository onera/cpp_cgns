#include "std_e/unit_test/doctest.hpp"
#include "cpp_cgns/tree_manip.hpp"

using namespace cgns;

TEST_CASE("find nodes") {
  tree t = {
    "A", "A_t", MT, {
      tree{"B0", "B_t", MT, {
          tree{"D", "A_t", MT, {}} } },
      tree{"B1", "B_t", MT, {
          tree{"D", "D_t", MT, {}} } } }
  };

  SUBCASE("get_nodes_by_matching") {
    auto ns = get_nodes_by_matching(t,"B_t/D");

    CHECK( ns.size() == 2 );

    const tree& n0 = ns[0];
    CHECK( n0.name  == "D" );
    CHECK( n0.label == "A_t" );

    const tree& n1 = ns[1];
    CHECK( n1.name  == "D" );
    CHECK( n1.label == "D_t" );
  }

  SUBCASE("get_nodes_by_matching -- const version") {
    const tree& ct = t;

    auto ns = get_nodes_by_matching(ct,"B_t/D");

    CHECK( ns.size() == 2 );

    const tree& n0 = ns[0];
    CHECK( n0.name  == "D" );
    CHECK( n0.label == "A_t" );

    const tree& n1 = ns[1];
    CHECK( n1.name  == "D" );
    CHECK( n1.label == "D_t" );
  }

  SUBCASE("get_node_by_matching") {
    auto n = get_node_by_matching(t,"B_t/D");

    CHECK( n.name  == "D" );
    CHECK( n.label == "A_t" );
  }

  SUBCASE("get_nodes_by_name") {
    auto ns = get_nodes_by_name(t,"D");

    CHECK( ns.size() == 2 );

    const tree& n0 = ns[0];
    CHECK( n0.name  == "D" );
    CHECK( n0.label == "A_t" );

    const tree& n1 = ns[1];
    CHECK( n1.name  == "D" );
    CHECK( n1.label == "D_t" );
  }

  SUBCASE("get_nodes_by_label") {
    auto ns = get_nodes_by_label(t,"A_t");

    CHECK( ns.size() == 2 );

    const tree& n0 = ns[0];
    CHECK( n0.name  == "A" );
    CHECK( n0.label == "A_t" );

    const tree& n1 = ns[1];
    CHECK( n1.name  == "D" );
    CHECK( n1.label == "A_t" );
  }
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
