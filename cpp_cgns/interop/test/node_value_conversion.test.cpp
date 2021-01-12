#include "cpp_cgns/interop/test/doctest_pybind.hpp"

#include "cpp_cgns/interop/node_value_conversion.hpp"
using namespace cgns;

PYBIND_TEST_CASE("view_as_node_value") {
  SUBCASE("Numeric types") {
    SUBCASE("I4") {
      std::vector<I4> v = {0,1,2};
      auto np_array = py::array(py::dtype("int32"),3,v.data());
      
      auto val = view_as_node_value(np_array);
      CHECK( val.data_type == "I4" );
      CHECK( val.dims == std::vector<I8>{3} );
      CHECK( val.data == np_array.data() );
      I4* data = (I4*)val.data;
      CHECK( data[0] == 0 );
      CHECK( data[1] == 1 );
      CHECK( data[2] == 2 );
    }
    SUBCASE("I8") {
      std::vector<I8> v = {10'000'000'000,1,2};
      auto np_array = py::array(py::dtype("int64"),3,v.data());
      
      auto val = view_as_node_value(np_array);
      CHECK( val.data_type == "I8" );
      CHECK( val.dims == std::vector<I8>{3} );
      CHECK( val.data == np_array.data() );
      I8* data = (I8*)val.data;
      CHECK( data[0] == 10'000'000'000 );
      CHECK( data[1] == 1 );
      CHECK( data[2] == 2 );
    }
    SUBCASE("R4") {
      std::vector<R4> v = {0.,1.5,2.5};
      auto np_array = py::array(py::dtype("float32"),3,v.data());
      
      auto val = view_as_node_value(np_array);
      CHECK( val.data_type == "R4" );
      CHECK( val.dims == std::vector<I8>{3} );
      CHECK( val.data == np_array.data() );
      R4* data = (R4*)val.data;
      CHECK( data[0] == 0.  );
      CHECK( data[1] == 1.5 );
      CHECK( data[2] == 2.5 );
    }
    SUBCASE("R8") {
      std::vector<R8> v = {0.,1.5,2.5};
      auto np_array = py::array(py::dtype("float64"),3,v.data());
      
      auto val = view_as_node_value(np_array);
      CHECK( val.data_type == "R8" );
      CHECK( val.dims == std::vector<I8>{3} );
      CHECK( val.data == np_array.data() );
      R8* data = (R8*)val.data;
      CHECK( data[0] == 0.  );
      CHECK( data[1] == 1.5 );
      CHECK( data[2] == 2.5 );
    }
    SUBCASE("C1") {
      std::vector<char> v = {'a','b','c'};
      auto np_array = py::array(py::dtype("b"),3,v.data());
      
      auto val = view_as_node_value(np_array);
      CHECK( val.data_type == "C1" );
      CHECK( val.dims == std::vector<I8>{3} );
      CHECK( val.data == np_array.data() );
      char* data = (char*)val.data;
      CHECK( data[0] == 'a' );
      CHECK( data[1] == 'b' );
      CHECK( data[2] == 'c' );
    }
  }

  SUBCASE("Multi-dimensional") {
    SUBCASE("I4") {
      std::vector<I4> v = {0,1,2,3,4,5};
      std::vector<I4> dims = {2,3};
      auto np_array = py::array_t<I4,py::array::f_style>(dims,v.data());
      
      auto val = view_as_node_value(np_array);
      CHECK( val.data_type == "I4" );
      CHECK( val.dims == std::vector<I8>{2,3} );
      CHECK( val.data == np_array.data() );
      I4* data = (I4*)val.data;
      CHECK( data[0] == 0 );
      CHECK( data[1] == 1 );
      CHECK( data[2] == 2 );
      CHECK( data[3] == 3 );
      CHECK( data[4] == 4 );
      CHECK( data[5] == 5 );
    }
  }
}
