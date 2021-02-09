#include "std_e/unit_test/doctest.hpp"
#include "cpp_cgns/node_manip.hpp"
#include "std_e/log.hpp"


using namespace cgns;

TEST_CASE("Create node value") {
  SUBCASE("one dimension") {
    node_value val = create_node_value({42,43,44});

    SUBCASE("direct access") {
      CHECK( val.data_type == "I4" );
      CHECK( val.dims == std::vector<I8>{3} );
      CHECK( ((I4*)data(val))[0] == 42 );
      CHECK( ((I4*)data(val))[1] == 43 );
      CHECK( ((I4*)data(val))[2] == 44 );
    }
    SUBCASE("view as span") {
      auto x = view_as_span<I4>(val); // the user must give the type (I4)
      CHECK( x[0] == 42 );
      CHECK( x[1] == 43 );
      CHECK( x[2] == 44 );
    }
  }

  SUBCASE("multiple dimensions") {
    node_value val = create_node_value({{2,3,4},
                                      {5,6,7}});

    SUBCASE("direct access") {
      CHECK( val.data_type == "I4" );
      CHECK( val.dims == std::vector<I8>{2,3} );
      // data is stored in Fortran order (as CGNS arrays)
      CHECK( ((I4*)data(val))[0] == 2 );
      CHECK( ((I4*)data(val))[1] == 5 );
      CHECK( ((I4*)data(val))[2] == 3 );
      CHECK( ((I4*)data(val))[3] == 6 );
      CHECK( ((I4*)data(val))[4] == 4 );
      CHECK( ((I4*)data(val))[5] == 7 );
    }

    SUBCASE("view as md_array_view") {
      auto x = view_as_md_array<I4,2>(val); // the user must give the type (I4) and the rank (2) of the array

      CHECK( x(0,0) == 2 ); CHECK( x(0,1) == 3 ); CHECK( x(0,2) == 4 );
      CHECK( x(1,0) == 5 ); CHECK( x(1,1) == 6 ); CHECK( x(1,2) == 7 );
    }
  }
}


TEST_CASE("Create node value from buffer_vector") {
  std_e::buffer_vector<I4> v;

  // use vector features (here, automatic growth with push_back)
  v.push_back(0);
  v.push_back(1);
  v.push_back(2);

  // once done, create a node_value from it
  node_value val = make_node_value(std::move(v));

  CHECK( val.data_type == "I4" );
  CHECK( val.dims == std::vector<I8>{3} );
  CHECK( ((I4*)data(val))[0] == 0 );
  CHECK( ((I4*)data(val))[1] == 1 );
  CHECK( ((I4*)data(val))[2] == 2 );
}


TEST_CASE("node_value to string") {
  auto val0 = create_node_value({0,1,2});
  auto val1 = create_node_value({{0},{1},{2}});
  auto val2 = create_node_value({{5,6},{7,8}});

  CHECK ( to_string(val0) == "[0,1,2]" );
  CHECK ( to_string(val1) == "[[0],[1],[2]]" );
  CHECK ( to_string(val2) == "[[5,6],[7,8]]" );
}

/* TODO
node_value create_node_value_1d(const Range& r, cgns_allocator& alloc) {
node_value create_node_value_1d(std::initializer_list<T> l, cgns_allocator& alloc) {
node_value create_string_node_value(const std::string& s, cgns_allocator& alloc) {
*/
