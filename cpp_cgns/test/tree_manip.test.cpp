#include "std_e/unit_test/doctest.hpp"
#include "cpp_cgns/tree_manip.hpp"

using namespace cgns;

TEST_CASE("find nodes") {
  tree t = {
    "A", "A_t", MT(), {
      tree{"B0", "B_t", node_value(std::vector<I4>{0,1,2}), {
          tree{"D", "A_t", node_value({{3.,4.,5.},{6.,7.,8.}}), {}} } },
      tree{"B1", "B_t", MT(), {
          tree{"D", "D_t", MT(), {}} } } }
  };

  SUBCASE("get_nodes_by_matching") {
    tree_range bs = get_children_by_label(t,"B_t");
    CHECK( bs.size() == 2 );
    CHECK( bs[0].name == "B0" );
    CHECK( bs[1].name == "B1" );

    SUBCASE("references") {
      //tree_range is made of references, we can assign to the origin tree through them
      bs[0].name = "X2";
      bs[1].name = "X3";

      CHECK( t.children[0].name == "X2" );
      CHECK( t.children[1].name == "X3" );
    }
  }

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
    auto& n = get_node_by_matching(t,"B_t/D");

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

  SUBCASE("get values") {
    std_e::span<I4> val_A = get_child_value_by_name<I4>(t,"B0");
    CHECK( val_A.size() == 3 );
    CHECK( val_A == std::vector{0,1,2} );

    CHECK_THROWS_AS( get_child_value_by_name<C1>(t,"B0"), const cgns_exception& );

    md_array_view<R8,2> val_D = get_node_value_by_matching<R8,2>(t,"B_t/D");
    CHECK( val_D.rank() == 2 );
    CHECK( val_D.extent(0) == 2 );
    CHECK( val_D.extent(1) == 3 );
    CHECK( val_D == md_array<R8,2>{{3.,4.,5.},{6.,7.,8.}} );
  }
}
