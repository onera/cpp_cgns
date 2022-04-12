#if __cplusplus > 201703L
#include "std_e/unit_test/doctest.hpp"
#include "cpp_cgns/cgns.hpp"
#include "cpp_cgns/base/node_value.hpp"
#include <memory>
#include "std_e/multi_array/utils.hpp"


using namespace cgns;


TEST_CASE("node_value construction") {
  SUBCASE("empty") {
    // [Sphinx Doc] empty node_value {
    node_value x;
    CHECK( x == MT() );
    CHECK( x.rank() == 0 );
    CHECK( x.data_type() == "MT" );
    // [Sphinx Doc] empty node_value }
  }

  SUBCASE("from string") {
    // [Sphinx Doc] node_value from string {
    node_value x("Alice & Bob");

    CHECK( x.rank() == 1 );
    CHECK( x.extent(0) == 11 );
    CHECK( x.data_type() == "C1" );
    CHECK( x(0) == 'A' );
    CHECK( x(10) == 'b' );
    // [Sphinx Doc] node_value from string }
  }

  SUBCASE("one dimension") {
    SUBCASE("from vector") {
      SUBCASE("I4") {
        node_value x(std::vector<I4>{42,43,44});
        CHECK( x.rank() == 1 );
        CHECK( x.extent(0) == 3 );
        CHECK( x.data_type() == "I4" );
        CHECK( x(0) == I4(42) );
        CHECK( x(1) == I4(43) );
        CHECK( x(2) == I4(44) );
      }
      SUBCASE("I8") {
        node_value x(std::vector<I8>{45,46,47});
        CHECK( x.rank() == 1 );
        CHECK( x.extent(0) == 3 );
        CHECK( x.data_type() == "I8" );
        CHECK( x(0) == I8(45) );
        CHECK( x(1) == I8(46) );
        CHECK( x(2) == I8(47) );
      }
      SUBCASE("R4") {
        node_value x(std::vector<R4>{3.14f,2.7f});
        CHECK( x.rank() == 1 );
        CHECK( x.extent(0) == 2 );
        CHECK( x.data_type() == "R4" );
        CHECK( x(0) == 3.14f );
        CHECK( x(1) == 2.7f );
      }
      SUBCASE("R8") {
        // [Sphinx Doc] node_value from vec R8 {
        node_value x(std::vector<R8>{1.5,0.3});
        CHECK( x.rank() == 1 );
        CHECK( x.extent(0) == 2 );
        CHECK( x.data_type() == "R8" );
        CHECK( x(0) == 1.5 );
        CHECK( x(1) == 0.3);
        // [Sphinx Doc] node_value from vec R8 }
      }
    }

    SUBCASE("from initializer list") {
      // [Sphinx Doc] node_value from init list {
      // Note: Shorthand for node_value(std::vector{...})
      // the underlying range of the node value will be a std::vector
      node_value x({42,43,44});

      CHECK( x.rank() == 1 );
      CHECK( x.extent(0) == 3 );
      CHECK( x.data_type() == "I4" );
      CHECK( x(0) == 42 );
      CHECK( x(1) == 43 );
      CHECK( x(2) == 44 );
      // [Sphinx Doc] node_value from init list }
    }

    SUBCASE("from span") {
      // [Sphinx Doc] node_value from span I4 {
      std::vector<I4> v = {42,43,44};
      std_e::span<I4> s(v.data(),v.size());
      node_value x(s);

      // Note: the node_value does not own its memory
      CHECK( x.data() == v.data() );

      CHECK( x.rank() == 1 );
      CHECK( x.extent(0) == 3 );
      CHECK( x.data_type() == "I4" );
      CHECK( x(0) == 42 );
      CHECK( x(1) == 43 );
      CHECK( x(2) == 44 );
      // [Sphinx Doc] node_value from span I4 }
    }
  }

  SUBCASE("multi dimensions") {
    SUBCASE("from md_array") {
      // [Sphinx Doc] node_value from md_array {
      node_value x(md_array<I4,2>{{0,1,2},{3,4,5}});
      CHECK( x.rank() == 2 );
      CHECK( x.extent(0) == 2 );
      CHECK( x.extent(1) == 3 );
      CHECK( x.data_type() == "I4" );
      CHECK( x(0,0) == 0 ); CHECK( x(0,1) == 1 ); CHECK( x(0,2) == 2 );
      CHECK( x(1,0) == 3 ); CHECK( x(1,1) == 4 ); CHECK( x(1,2) == 5 );
      // [Sphinx Doc] node_value from md_array }
    }

    SUBCASE("from initializer list") {
      // [Sphinx Doc] node_value from multi dim init_list {
      // Note: Shorthand for node_value(md_array{...})
      // the underlying range of the node value will be a vector
      node_value x({{0,1,2},{3,4,5}});

      CHECK( x.rank() == 2 );
      CHECK( x.extent(0) == 2 );
      CHECK( x.extent(1) == 3 );
      CHECK( x.data_type() == "I4" );
      CHECK( x(0,0) == 0 ); CHECK( x(0,1) == 1 ); CHECK( x(0,2) == 2 );
      CHECK( x(1,0) == 3 ); CHECK( x(1,1) == 4 ); CHECK( x(1,2) == 5 );
      // [Sphinx Doc] node_value from multi dim init_list }
    }

    SUBCASE("from md_array_view") {
      // [Sphinx Doc] node_value from md_array_view {
      std::vector<I4> v = {0,1,2,3,4,5};
      md_array_shape<2> shape{{3,2}};
      auto s = std_e::make_span<I4>(v.data(),shape.size());
      md_array_view<I4,2> ma(s,shape);
      node_value x(std::move(ma));

      // Note: the node_value does not own its memory
      CHECK( x.data() == v.data() );

      CHECK( x.rank() == 2 );
      CHECK( x.extent(0) == 3 );
      CHECK( x.extent(1) == 2 );
      CHECK( x.data_type() == "I4" );
      CHECK( x(0,0) == 0 ); CHECK( x(0,1) == 3 );
      CHECK( x(1,0) == 1 ); CHECK( x(1,1) == 4 );
      CHECK( x(2,0) == 2 ); CHECK( x(2,1) == 5 );
      // [Sphinx Doc] node_value from md_array_view }
    }

    SUBCASE("from vector + reshape") {
      // [Sphinx Doc] node_value reshape {
      node_value x(std::vector<I4>{0,1,2,3,4,5});
      cons_reshape(x,{2,3}); // `cons_` mean conservative: the total size should not change
      CHECK( x.rank() == 2 );
      CHECK( x.extent(0) == 2 );
      CHECK( x.extent(1) == 3 );
      CHECK( x.data_type() == "I4" );
      // Note: Fortran order
      CHECK( x(0,0) == 0 ); CHECK( x(0,1) == 2 ); CHECK( x(0,2) == 4 );
      CHECK( x(1,0) == 1 ); CHECK( x(1,1) == 3 ); CHECK( x(1,2) == 5 );
      // [Sphinx Doc] node_value reshape }
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
  SUBCASE("Principle of scalar_ref") {
    // [Sphinx Doc] node_value scalar_ref {
    node_value x({1.5,0.3});

    scalar_ref x0 = x(0);

    CHECK( x0.data_type() == "R8" );

    x0 = 2.8; // x0 has reference semantics: assigning to it assigns to the underlying value
    CHECK( x == std::vector{2.8,0.3} );

    // promoting (here from R4 to R8) is allowed
    x0 = R4(3.2);
    CHECK( x0.data_type() == "R8" ); // the type didn't change, the R4 was converted to an R8
    CHECK( x(0) == doctest::Approx(3.2) ); // Approx because 3.2f != 3.2

    // narrowing (here from I4 to R8) is forbidden
    CHECK_THROWS_AS( x0 = I4(0)  ,  const cgns_exception& );
    // [Sphinx Doc] node_value scalar_ref }
  }

  SUBCASE("C1") {
    node_value x("Alice & Bob");
    x(0) = 'C';
    x(1) = 'a';
    x(2) = 'r';
    x(3) = 'o';
    x(4) = 'l';

    CHECK( x == std::string("Carol & Bob") );

    SUBCASE("forbidded conversions") {
      CHECK_THROWS_AS( x(0) = I4(0 )  ,  const cgns_exception& );
      CHECK_THROWS_AS( x(0) = I8(0 )  ,  const cgns_exception& );
      CHECK_THROWS_AS( x(0) = R4(0.)  ,  const cgns_exception& );
      CHECK_THROWS_AS( x(0) = R8(0.)  ,  const cgns_exception& );
    }
  }

  SUBCASE("I4") {
    node_value x(std::vector<I4>{42,43,44});

    x(0) = I4(100);

    CHECK( x == std::vector<I4>({100,43,44}) );

    SUBCASE("forbidded conversions") {
      CHECK_THROWS_AS( x(0) = C1('A')  ,  const cgns_exception& );
      CHECK_THROWS_AS( x(0) = I8(0)    ,  const cgns_exception& );
      CHECK_THROWS_AS( x(0) = R4(0.)   ,  const cgns_exception& );
      CHECK_THROWS_AS( x(0) = R8(0.)   ,  const cgns_exception& );
    }
  }
  SUBCASE("I8") {
    node_value x(std::vector<I8>{45,46,47});

    x(0) = (I8)10'000'000'000;
    x(2) = (I4)100; // conversion from R4 to R8 is allowed (no narrowing)

    CHECK( x == std::vector<I8>({10'000'000'000,46,100}) );

    SUBCASE("forbidded conversions") {
      CHECK_THROWS_AS( x(0) = C1('A')  ,  const cgns_exception& );
      CHECK_THROWS_AS( x(0) = R4(0.)   ,  const cgns_exception& );
      CHECK_THROWS_AS( x(0) = R8(0.)   ,  const cgns_exception& );
    }
  }

  SUBCASE("R4") {
    node_value x(std::vector<R4>{3.14f,2.7f});

    x(1) = R4(1.5);

    CHECK( x == std::vector<R4>({3.14f,1.5f}) );

    SUBCASE("forbidded conversions") {
      CHECK_THROWS_AS( x(0) = C1('A')  ,  const cgns_exception& );
      CHECK_THROWS_AS( x(0) = I4(0)    ,  const cgns_exception& );
      CHECK_THROWS_AS( x(0) = I8(0)    ,  const cgns_exception& );
      CHECK_THROWS_AS( x(0) = R8(0.)   ,  const cgns_exception& );
    }
  }

  SUBCASE("R8") {
    node_value x(std::vector<R8>{1.5,0.3});

    x(0) = R8(0.66);
    x(1) = R4(0.2f); // conversion from R4 to R8 is allowed (no narrowing)

    CHECK( x(0) == 0.66 );
    CHECK( x(1) == doctest::Approx(0.2) ); // Approx because 0.2f != 0.2

    SUBCASE("forbidded conversions") {
      CHECK_THROWS_AS( x(0) = C1('A')  ,  const cgns_exception& );
      CHECK_THROWS_AS( x(0) = I4(0)    ,  const cgns_exception& );
      CHECK_THROWS_AS( x(0) = I8(0)    ,  const cgns_exception& );
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
#endif // C++>17
