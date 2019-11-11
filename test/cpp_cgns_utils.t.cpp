#include "doctest/doctest.hpp"
#include "cpp_cgns/cpp_cgns_utils.hpp"


using namespace cpp_cgns;


TEST_CASE("View multi_array as cgns node_value") {
  std_e::dyn_multi_array<I4,I8,2> x = {
    {2,3,4},
    {5,6,7}
  };
  node_value val = view_as_node_value(x);


  SUBCASE("data is stored in fortran order") {
    CHECK( val.data_type == "I4" );
    CHECK( val.dims == std::vector<I8>{2,3} );
    CHECK( ((I4*)val.data)[0] == 2 );
    CHECK( ((I4*)val.data)[1] == 5 );
    CHECK( ((I4*)val.data)[2] == 3 );
    CHECK( ((I4*)val.data)[3] == 6 );
    CHECK( ((I4*)val.data)[4] == 4 );
    CHECK( ((I4*)val.data)[5] == 7 );
  }

  SUBCASE("data is shared") {
    ((I4*)val.data)[2] = 42;
    x(1,0) = 43;

    CHECK( ((I4*)val.data)[0] == 2 );
    CHECK( ((I4*)val.data)[1] == 43);
    CHECK( ((I4*)val.data)[2] == 42);
    CHECK( ((I4*)val.data)[3] == 6 );
    CHECK( ((I4*)val.data)[4] == 4 );
    CHECK( ((I4*)val.data)[5] == 7 );

    CHECK( x(0,0) == 2 );
    CHECK( x(1,0) == 43);
    CHECK( x(0,1) == 42);
    CHECK( x(1,1) == 6 );
    CHECK( x(0,2) == 4 );
    CHECK( x(1,2) == 7 );
  }
}

TEST_CASE("View cgns_vector as cgns node_value") {
  cgns_allocator alloc;
  auto x = make_cgns_vector({42,43,44},alloc);

  node_value val = view_as_node_value(x);

  SUBCASE("data is stored") {
    CHECK( val.data_type == "I4" );
    CHECK( val.dims == std::vector<I8>{3} );
    CHECK( ((I4*)val.data)[0] == 42 );
    CHECK( ((I4*)val.data)[1] == 43 );
    CHECK( ((I4*)val.data)[2] == 44 );
  }

  SUBCASE("data is shared") {
    x[1] = 50;
    ((I4*)val.data)[2] = 51;

    CHECK( ((I4*)val.data)[0] == 42);
    CHECK( ((I4*)val.data)[1] == 50);
    CHECK( ((I4*)val.data)[2] == 51);

    CHECK( x[0] == 42);
    CHECK( x[1] == 50);
    CHECK( x[2] == 51);
  }
}


TEST_CASE("View node_value as md_array_view") {
  std::vector<I4> values = {2,3,4,5,6,7};
  node_value val = {"I4",{2,3},values.data()};

  auto x = view_as_md_array<I4,2>(val); // the user must give the type (I4) and the rank (2) of the array

  SUBCASE("data is stored in fortran order") {
    CHECK( x(0,0) == 2 );
    CHECK( x(1,0) == 3 );
    CHECK( x(0,1) == 4 );
    CHECK( x(1,1) == 5 );
    CHECK( x(0,2) == 6 );
    CHECK( x(1,2) == 7 );
  }

  SUBCASE("data is shared") {
    x(1,0) = 42;
    ((I4*)val.data)[2] = 43;

    CHECK( x(0,0) == 2 );
    CHECK( x(1,0) == 42);
    CHECK( x(0,1) == 43);
    CHECK( x(1,1) == 5 );
    CHECK( x(0,2) == 6 );
    CHECK( x(1,2) == 7 );
  }
}

TEST_CASE("View node_value as span") {
  std::vector<I4> values = {42,43,44};
  I8 data_size = values.size();
  node_value val = {"I4",{data_size},values.data()};

  auto x = view_as_span<I4>(val); // the user must give the type (I4)

  CHECK( x[0] == 42 );
  CHECK( x[1] == 43 );
  CHECK( x[2] == 44 );
}

/* TODO
 * allocate small arrays
node_value create_node_value_1d(const Range& r, cgns_allocator& alloc) {
node_value create_node_value_1d(std::initializer_list<T> l, cgns_allocator& alloc) {
node_value create_string_node_value(const std::string& s, cgns_allocator& alloc) {

 * allocate_md_array
 * to_string
 * deep_copy
 */
