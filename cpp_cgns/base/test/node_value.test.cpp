#include "std_e/unit_test/doctest.hpp"
#include "cpp_cgns/cgns.hpp"
#include "cpp_cgns/base/node_value.hpp"
#include <memory>


using namespace cgns;


TEST_CASE("node_value construction") {
  SUBCASE("empty") {
    node_value x;
    CHECK( x == MT() );
    CHECK( x.rank() == 0 );
    CHECK( x.data_type() == "MT" );
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
      node_value x2(std::vector<R4>{3.14f,2.7f});
      node_value x3(std::vector<R8>{1.5,0.3});

      CHECK( x0.rank() == 1 );
      CHECK( x1.rank() == 1 );
      CHECK( x2.rank() == 1 );
      CHECK( x3.rank() == 1 );
      CHECK( x0.extent(0) == 3 );
      CHECK( x1.extent(0) == 3 );
      CHECK( x2.extent(0) == 2 );
      CHECK( x3.extent(0) == 2 );
      CHECK( x0.data_type() == "I4" );
      CHECK( x1.data_type() == "I8" );
      CHECK( x2.data_type() == "R4" );
      CHECK( x3.data_type() == "R8" );
      CHECK( x0(0) == 42 );
      CHECK( x0(1) == 43 );
      CHECK( x0(2) == 44 );
      CHECK( x1(0) == 45l );
      CHECK( x1(1) == 46l );
      CHECK( x1(2) == 47l );
      CHECK( x2(0) == 3.14f );
      CHECK( x2(1) == 2.7f );
      CHECK( x3(0) == 1.5 );
      CHECK( x3(1) == 0.3);
    }

    SUBCASE("from initializer list") {
      // Note: Shorthand for node_value(std::vector{...})
      // the underlying range of the node value will be a std::vector
      node_value x({42,43,44});

      CHECK( x.rank() == 1 );
      CHECK( x.extent(0) == 3 );
      CHECK( x.data_type() == "I4" );
      CHECK( x(0) == 42 );
      CHECK( x(1) == 43 );
      CHECK( x(2) == 44 );
    }

    SUBCASE("from span") {
      std::vector<I4> v = {42,43,44};
      std_e::span<I4> s(v.data(),v.size());
      node_value x(s);

      // Note: in this case, the node_value does not own its memory
      CHECK( x.data() == v.data() );

      CHECK( x.rank() == 1 );
      CHECK( x.extent(0) == 3 );
      CHECK( x.data_type() == "I4" );
      CHECK( x(0) == 42 );
      CHECK( x(1) == 43 );
      CHECK( x(2) == 44 );
    }
  }

  SUBCASE("multi dimensions") {
    SUBCASE("from md_array") {
      node_value x(md_array<I4,2>{{0,1,2},{3,4,5}});
      CHECK( x.rank() == 2 );
      CHECK( x.extent(0) == 2 );
      CHECK( x.extent(1) == 3 );
      CHECK( x.data_type() == "I4" );
      CHECK( x(0,0) == 0 ); CHECK( x(0,1) == 1 ); CHECK( x(0,2) == 2 );
      CHECK( x(1,0) == 3 ); CHECK( x(1,1) == 4 ); CHECK( x(1,2) == 5 );
    }

    SUBCASE("from initializer list") {
      // Note: Shorthand for node_value(md_array{...})
      // the underlying range of the node value will be a vector
      node_value x({{0,1,2},{3,4,5}});

      CHECK( x.rank() == 2 );
      CHECK( x.extent(0) == 2 );
      CHECK( x.extent(1) == 3 );
      CHECK( x.data_type() == "I4" );
      CHECK( x(0,0) == 0 ); CHECK( x(0,1) == 1 ); CHECK( x(0,2) == 2 );
      CHECK( x(1,0) == 3 ); CHECK( x(1,1) == 4 ); CHECK( x(1,2) == 5 );
    }

    SUBCASE("from md_array_view") {
      std::vector<I4> v = {0,1,2,3,4,5};
      md_array_shape<2> shape{{3,2}};
      auto s = std_e::make_span<I4>(v.data(),shape.size());
      md_array_view<I4,2> ma(s,shape);
      node_value x(std::move(ma));

      // Note: in this case, the node_value does not own its memory
      CHECK( x.data() == v.data() );

      CHECK( x.rank() == 2 );
      CHECK( x.extent(0) == 3 );
      CHECK( x.extent(1) == 2 );
      CHECK( x.data_type() == "I4" );
      CHECK( x(0,0) == 0 ); CHECK( x(0,1) == 3 );
      CHECK( x(1,0) == 1 ); CHECK( x(1,1) == 4 );
      CHECK( x(2,0) == 2 ); CHECK( x(2,1) == 5 );
    }
  }

  SUBCASE("const ") {
    const node_value x(std::vector<I4>{42,43,44});

    CHECK( x.rank() == 1 );
    CHECK( x.extent(0) == 3 );
    CHECK( x.data_type() == "I4" );
    CHECK( x(0) == 42 );
    CHECK( x(1) == 43 );
    CHECK( x(2) == 44 );
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
    SUBCASE("with other node_value") {
      CHECK( x0==x0 );
      CHECK( x0==x1 );
      CHECK( x0!=x2 );
      CHECK( x0!=x3 );
      CHECK( x0==x4 );
      CHECK( x0==x5 );

      CHECK( x4==x5 );
    }
    SUBCASE("with vector or span") {
      CHECK( x0 == v );
      CHECK( v == x0 );
      CHECK( x0 == std_e::make_span(v) );
      CHECK( std_e::make_span(v) == x0 );

      CHECK( x2 != v );

      node_value x_multi_dim({{0,2},{1,3}});
      CHECK( x_multi_dim != v ); // not equal because multi_dim
    }
  }
}

TEST_CASE("node_value assignments") {

  SUBCASE("C1") {
    node_value x("Alice & Bob");
    x(0) = 'C';
    x(1) = 'a';
    x(2) = 'r';
    x(3) = 'o';
    x(4) = 'l';

    CHECK( x == std::string("Carol & Bob") );

    SUBCASE("forbidded conversions") {
      CHECK_THROWS_AS(x(0) = I4(0), const std::bad_variant_access&);
      CHECK_THROWS_AS(x(0) = I8(0), const std::bad_variant_access&);
      CHECK_THROWS_AS(x(0) = R4(0.), const std::bad_variant_access&);
      CHECK_THROWS_AS(x(0) = R8(0.), const std::bad_variant_access&);
    }
  }

  SUBCASE("I4") {
    node_value x(std::vector<I4>{42,43,44});

    x(0) = I4(100);

    CHECK( x == std::vector<I4>({100,43,44}) );

    SUBCASE("forbidded conversions") {
      CHECK_THROWS_AS(x(0) = C1('A'), const std::bad_variant_access&);
      CHECK_THROWS_AS(x(0) = I8(0), const std::bad_variant_access&);
      CHECK_THROWS_AS(x(0) = R4(0.), const std::bad_variant_access&);
      CHECK_THROWS_AS(x(0) = R8(0.), const std::bad_variant_access&);
    }
  }
  SUBCASE("I8") {
    node_value x(std::vector<I8>{45,46,47});

    x(0) = (I8)10'000'000'000;
    x(2) = (I4)100; // conversion from R4 to R8 is allowed (no narrowing)

    CHECK( x == std::vector<I8>({10'000'000'000,46,100}) );

    SUBCASE("forbidded conversions") {
      CHECK_THROWS_AS(x(0) = C1('A'), const std::bad_variant_access&);
      CHECK_THROWS_AS(x(0) = R4(0.), const std::bad_variant_access&);
      CHECK_THROWS_AS(x(0) = R8(0.), const std::bad_variant_access&);
    }
  }

  SUBCASE("R4") {
    node_value x(std::vector<R4>{3.14f,2.7f});

    x(1) = R4(1.5);

    CHECK( x == std::vector<R4>({3.14f,1.5f}) );

    SUBCASE("forbidded conversions") {
      CHECK_THROWS_AS(x(0) = C1('A'), const std::bad_variant_access&);
      CHECK_THROWS_AS(x(0) = I4(0), const std::bad_variant_access&);
      CHECK_THROWS_AS(x(0) = I8(0), const std::bad_variant_access&);
      CHECK_THROWS_AS(x(0) = R8(0.), const std::bad_variant_access&);
    }
  }

  SUBCASE("R8") {
    node_value x(std::vector<R8>{1.5,0.3});

    x(0) = R8(0.66);
    x(1) = R4(0.2f); // conversion from R4 to R8 is allowed (no narrowing)

    CHECK( x(0) == 0.66 );
    CHECK( x(1) == doctest::Approx(0.2) ); // Approx because 0.2f != 0.2

    SUBCASE("forbidded conversions") {
      CHECK_THROWS_AS(x(0) = C1('A'), const std::bad_variant_access&);
      CHECK_THROWS_AS(x(0) = I4(0), const std::bad_variant_access&);
      CHECK_THROWS_AS(x(0) = I8(0), const std::bad_variant_access&);
    }
  }

  SUBCASE("multi dimensional") {
    node_value x({{0,1,2},{3,4,5}});

    x(0,0) = I4( 0);  x(0,1) = I4(10);  x(0,2) = I4(20);
    x(1,0) = I4(30);  x(1,1) = I4(40);  x(1,2) = I4(50);

    CHECK( x(0,0) ==  0 ); CHECK( x(0,1) == 10 ); CHECK( x(0,2) == 20 );
    CHECK( x(1,0) == 30 ); CHECK( x(1,1) == 40 ); CHECK( x(1,2) == 50 );
  }
}
