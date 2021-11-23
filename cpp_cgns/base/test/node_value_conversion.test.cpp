#include "std_e/unit_test/doctest.hpp"
#include "cpp_cgns/base/node_value_conversion.hpp"
#include "std_e/log.hpp"


using namespace cgns;

TEST_CASE("view_as_span") {
  SUBCASE("non const") {
    node_value val = {42,43,44};
    std_e::span<I4> x = view_as_span<I4>(val);

    SUBCASE("values") {
      CHECK( x[0] == 42 );
      CHECK( x[1] == 43 );
      CHECK( x[2] == 44 );
    }
    SUBCASE("changes are forwarded") {
      x[0] = 100;
      CHECK( val[0] == 100 );
      CHECK( val[1] == 43 );
      CHECK( val[2] == 44 );
    }
  }

  SUBCASE("const") {
    const node_value val = {42,43,44};
    std_e::span<const I4> x = view_as_span<I4>(val);
    CHECK( x[0] == 42 );
    CHECK( x[1] == 43 );
    CHECK( x[2] == 44 );
  }
}

TEST_CASE("view_as_md_array") {
  SUBCASE("non const") {
    node_value val = {{2,3,4},
                      {5,6,7}};
    md_array_view<I4,2> x = view_as_md_array<I4,2>(val);
    SUBCASE("values") {
      CHECK( x.rank() == 2 );
      CHECK( x.extent(0) == 2 );
      CHECK( x.extent(1) == 3 );
      CHECK( x(0,0) == 2 ); CHECK( x(0,1) == 3 ); CHECK( x(0,2) == 4 );
      CHECK( x(1,0) == 5 ); CHECK( x(1,1) == 6 ); CHECK( x(1,2) == 7 );
    }
    SUBCASE("changes are forwarded") {
      x(1,0) = 100;
      CHECK( val(0,0) == 2   ); CHECK( val(0,1) == 3 ); CHECK( val(0,2) == 4 );
      CHECK( val(1,0) == 100 ); CHECK( val(1,1) == 6 ); CHECK( val(1,2) == 7 );
    }
  }
  SUBCASE("const") {
    const node_value val = {{2,3,4},
                            {5,6,7}};
    md_array_view<const I4,2> x = view_as_md_array<I4,2>(val);
    SUBCASE("values") {
      CHECK( x.rank() == 2 );
      CHECK( x.extent(0) == 2 );
      CHECK( x.extent(1) == 3 );
      CHECK( x(0,0) == 2 ); CHECK( x(0,1) == 3 ); CHECK( x(0,2) == 4 );
      CHECK( x(1,0) == 5 ); CHECK( x(1,1) == 6 ); CHECK( x(1,2) == 7 );
    }
  }

  SUBCASE("view with another shape") {
    const node_value val = {{2,3,4},
                            {5,6,7}};
    md_array_view<const I4,2> x = view_as_md_array<I4,2>(val,{1,6});

    CHECK( x.rank() == 2 );
    CHECK( x.extent(0) == 1 );
    CHECK( x.extent(1) == 6 );
    CHECK( x(0,0) == 2 );
    CHECK( x(0,1) == 5 );
    CHECK( x(0,2) == 3 );
    CHECK( x(0,3) == 6 );
    CHECK( x(0,4) == 4 );
    CHECK( x(0,5) == 7 );
  }
}



TEST_CASE("node_value to string") {
  node_value val0 = {0,1,2};
  node_value val1 = {{0},{1},{2}};
  node_value val2 = {{5,6},{7,8}};
  node_value val3 = "Totoro";

  CHECK ( to_string(val0) == "[0,1,2]" );
  CHECK ( to_string(val1) == "[[0],[1],[2]]" );
  CHECK ( to_string(val2) == "[[5,6],[7,8]]" );
  CHECK ( to_string(val3) == "Totoro" );
}
