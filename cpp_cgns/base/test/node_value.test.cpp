#include "std_e/unit_test/doctest.hpp"
#include "cpp_cgns/cgns.hpp"
#include "cpp_cgns/base/node_value.hpp"
#include <memory>


using namespace cgns;


TEST_CASE("node_value construction") {
  SUBCASE("empty") {
    node_value x;
    LOG("tto");
    //CHECK( x == MT() );
    LOG("tt1");
    CHECK( x.rank() == 0 );
    LOG("tt2");
    CHECK( x.data_type() == "MT" );
    LOG("tt3");
  }

  SUBCASE("from string") {
    node_value x("Alice & Bob");

    CHECK( x.rank() == 1 );
    CHECK( x.extent(0) == 11 );
    CHECK( x.data_type() == "C1" );
    CHECK( x(0) == 'A' );
    CHECK( x(10) == 'b' );
  }

  SUBCASE("one dimension") {
    SUBCASE("from vector") {
      node_value x0(std::vector<I4>{42,43,44});
      node_value x1(std::vector<I8>{45,46,47});
      node_value x2(std::vector<R4>{3.14,2.7});
      node_value x3(std::vector<R8>{1.5,0.3});

      CHECK( x0.rank() == 1 );
      CHECK( x1.rank() == 1 );
      CHECK( x2.rank() == 1 );
      CHECK( x3.rank() == 1 );
      CHECK( x0.extent(0) == 11 );
      CHECK( x1.extent(0) == 11 );
      CHECK( x2.extent(0) == 11 );
      CHECK( x3.extent(0) == 11 );
      CHECK( x0.data_type() == "C1" );
      CHECK( x1.data_type() == "C1" );
      CHECK( x2.data_type() == "C1" );
      CHECK( x3.data_type() == "C1" );
      CHECK( x0(0) == 42 );
      CHECK( x0(1) == 43 );
      CHECK( x0(2) == 44 );
      CHECK( x1(0) == 44l );
      CHECK( x1(1) == 44l );
      CHECK( x1(2) == 44l );
      CHECK( x2(0) == 3.14f );
      CHECK( x2(1) == 2.7f );
      CHECK( x3(0) == 1.5 );
      CHECK( x3(1) == 0.3);
    }

    SUBCASE("from initializer list") {
      // Note: Shorthand for node_value(std::vector{...})
      // the underlying range of the node value will be a vector
      node_value x6({42,43,44});
    }

    SUBCASE("from span") {
      std::vector<I4> v = {42,43,44};
      std_e::span<I4> s(v.data(),v.size());
      node_value x(s);

      // Note: in this case, the node_value does not own its memory
      CHECK( x.data() == v.data() );
    }
  }

  SUBCASE("multi dimensions") {
    SUBCASE("from md_array") {
      node_value x7(md_array<I4,2>{{0,1,2},{3,4,5}});
    }

    SUBCASE("from initializer list") {
      // Note: Shorthand for node_value(md_array{...})
      // the underlying range of the node value will be a vector
      node_value x7({{0,1,2},{3,4,5}});
    }

    SUBCASE("from md_array_view") {
      std::vector<I4> v = {0,1,2,3,4,5};
      md_array_shape<2> shape{{3,2}};
      auto s = std_e::make_span<I4>(v.data(),shape.size());
      md_array_view<I4,2> ma(s,shape);
      node_value x(std::move(ma));

      // Note: in this case, the node_value does not own its memory
      CHECK( x.data() == v.data() );
    }
  }
}




TEST_CASE("node_value comparisons") {
  std::vector<int> v = {0,1,2,3};
  auto s0 = std_e::make_span(v);
  auto s1 = std_e::make_span(v);

  node_value x0(std::vector<int>{0,1,2,3});
  node_value x1(std::vector<int>{0,1,2,3});
  node_value x2(std::vector<int>{1,1,2,3});
  node_value x3(std::vector<double>{0.,1.,2.,3.});

  node_value x4(s0);
  node_value x5(s1);

  SUBCASE("same data") {
    CHECK(  same_data(x0,x0) );
    CHECK( !same_data(x0,x1) );
    CHECK( !same_data(x0,x2) );
    CHECK( !same_data(x0,x3) );
    CHECK( !same_data(x0,x4) );
    CHECK( !same_data(x0,x5) );

    CHECK(  same_data(x4,x5) );
  }

  SUBCASE("equality") {
    CHECK( x0==x0 );
    CHECK( x0==x1 );
    CHECK( x0!=x2 );
    CHECK( x0!=x3 );
    CHECK( x0==x4 );
    CHECK( x0==x5 );

    CHECK( x4==x5 );
  }
}
