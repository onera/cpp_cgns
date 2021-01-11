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
