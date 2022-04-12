#if __cplusplus > 201703L
#include "std_e/unit_test/doctest.hpp"
#include "cpp_cgns/base/node_value_conversion.hpp"
#include "std_e/multi_array/utils.hpp"


using namespace cgns;

TEST_CASE("view_as_span") {
  SUBCASE("non const") {
    // [Sphinx Doc] node_value as span {
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
    // [Sphinx Doc] node_value as span }
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
    // [Sphinx Doc] node_value as md_array {
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
    SUBCASE("values changes are forwarded") {
      x(1,0) = 100;
      CHECK( val(0,0) == 2   ); CHECK( val(0,1) == 3 ); CHECK( val(0,2) == 4 );
      CHECK( val(1,0) == 100 ); CHECK( val(1,1) == 6 ); CHECK( val(1,2) == 7 );
    }
    SUBCASE("dimension changes are NOT forwarded") {
      x.extent() = {3,2};

      // dimension changes are working for the view
      CHECK( x.extent(0) == 3 );
      CHECK( x.extent(1) == 2 );
      CHECK( x(0,0) == 2 ); CHECK( x(0,1) == 6 );
      CHECK( x(1,0) == 5 ); CHECK( x(1,1) == 4 );
      CHECK( x(2,0) == 3 ); CHECK( x(2,1) == 7 );

      // but the node_value remains unaffected
      CHECK( val.rank() == 2 );
      CHECK( val.extent(0) == 2 );
      CHECK( val.extent(1) == 3 );
      CHECK( val(0,0) == 2 ); CHECK( val(0,1) == 3 ); CHECK( val(0,2) == 4 );
      CHECK( val(1,0) == 5 ); CHECK( val(1,1) == 6 ); CHECK( val(1,2) == 7 );
    }
    // [Sphinx Doc] node_value as md_array }
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

TEST_CASE("view_as_array") {
  // [Sphinx Doc] node_value view_as_array {
  node_value x0 = {0,1,2};
  node_value x1 = {{3,4,5}};
  node_value x2 = {{6},{7},{8}};
  node_value x3 = {{ 9,11,13},
                   {10,12,14}};

  CHECK( view_as_array<I4>(x0) == std::vector{0,1,2} );
  CHECK( view_as_array<I4,2>(x1) == md_array<I4,2>{{3,4,5}} );
  CHECK( view_as_array<I4,2>(x2) == md_array<I4,2>{{6},{7},{8}} );
  CHECK( view_as_array<I4,2>(x3) == md_array<I4,2>{{9,11,13},{10,12,14}} );

  // multi-dim arrays which have a 1D shape can be seen as 1D
  CHECK( view_as_array<I4>(x1) == std::vector{3,4,5} );
  CHECK( view_as_array<I4>(x2) == std::vector{6,7,8} );

  // other rank changes result in an error
  CHECK_THROWS_AS( (view_as_array<I4,2>(x0))  ,  const cgns_exception& );
  CHECK_THROWS_AS(  view_as_array<I4>(x3)     ,  const cgns_exception& );
  // [Sphinx Doc] node_value view_as_array }
}



TEST_CASE("node_value to string") {
  // [Sphinx Doc] node_value to_string {
  node_value val0 = {0,1,2};
  node_value val1 = {{0},{1},{2}};
  node_value val2 = {{5,6},{7,8}};
  node_value val3 = "My_string";

  CHECK ( to_string(val0) == "I4[0,1,2]" );
  CHECK ( to_string(val1) == "I4[[0],[1],[2]]" );
  CHECK ( to_string(val2) == "I4[[5,6],[7,8]]" );
  CHECK ( to_string(val3) == "My_string" );
  // [Sphinx Doc] node_value to_string }
}
#endif // C++>17
