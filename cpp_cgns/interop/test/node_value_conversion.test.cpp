#include "cpp_cgns/interop/test/doctest_pybind.hpp"

#include "cpp_cgns/interop/node_value_conversion.hpp"
#include "cpp_cgns/node_manip.hpp"
using namespace cgns;

PYBIND_TEST_CASE("view_as_node_value") {
  SUBCASE("Numeric types") {
    SUBCASE("I4") {
      std::vector<I4> v = {0,1,2};
      auto np_array = py::array(py::dtype("int32"),3,v.data());
      
      auto val = view_as_node_value(np_array);
      CHECK( val.data_type == "I4" );
      CHECK( val.dims == std::vector<I8>{3} );
      CHECK( data(val) == np_array.data() );
      I4* ptr = (I4*)data(val);
      CHECK( ptr[0] == 0 );
      CHECK( ptr[1] == 1 );
      CHECK( ptr[2] == 2 );
    }
    SUBCASE("I8") {
      std::vector<I8> v = {10'000'000'000,1,2};
      auto np_array = py::array(py::dtype("int64"),3,v.data());
      
      auto val = view_as_node_value(np_array);
      CHECK( val.data_type == "I8" );
      CHECK( val.dims == std::vector<I8>{3} );
      CHECK( data(val) == np_array.data() );
      I8* ptr = (I8*)data(val);
      CHECK( ptr[0] == 10'000'000'000 );
      CHECK( ptr[1] == 1 );
      CHECK( ptr[2] == 2 );
    }
    SUBCASE("R4") {
      std::vector<R4> v = {0.,1.5,2.5};
      auto np_array = py::array(py::dtype("float32"),3,v.data());
      
      auto val = view_as_node_value(np_array);
      CHECK( val.data_type == "R4" );
      CHECK( val.dims == std::vector<I8>{3} );
      CHECK( data(val) == np_array.data() );
      R4* ptr = (R4*)data(val);
      CHECK( ptr[0] == 0.  );
      CHECK( ptr[1] == 1.5 );
      CHECK( ptr[2] == 2.5 );
    }
    SUBCASE("R8") {
      std::vector<R8> v = {0.,1.5,2.5};
      auto np_array = py::array(py::dtype("float64"),3,v.data());
      
      auto val = view_as_node_value(np_array);
      CHECK( val.data_type == "R8" );
      CHECK( val.dims == std::vector<I8>{3} );
      CHECK( data(val) == np_array.data() );
      R8* ptr = (R8*)data(val);
      CHECK( ptr[0] == 0.  );
      CHECK( ptr[1] == 1.5 );
      CHECK( ptr[2] == 2.5 );
    }
    SUBCASE("C1") {
      std::vector<char> v = {'a','b','c'};
      auto np_array = py::array(py::dtype("|S1"),3,v.data());
      
      auto val = view_as_node_value(np_array);
      CHECK( val.data_type == "C1" );
      CHECK( val.dims == std::vector<I8>{3} );
      CHECK( data(val) == np_array.data() );
      char* ptr = (char*)data(val);
      CHECK( ptr[0] == 'a' );
      CHECK( ptr[1] == 'b' );
      CHECK( ptr[2] == 'c' );
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
      CHECK( data(val) == np_array.data() );
      I4* ptr = (I4*)data(val);
      CHECK( ptr[0] == 0 );
      CHECK( ptr[1] == 1 );
      CHECK( ptr[2] == 2 );
      CHECK( ptr[3] == 3 );
      CHECK( ptr[4] == 4 );
      CHECK( ptr[5] == 5 );
    }
  }
}

PYBIND_TEST_CASE("to_np_array") {
  auto val = create_node_value({{0,1,2},{3,4,5}});
  py::array np_arr = to_np_array(val);

  // Check the data is right
  CHECK( std::string(py::str(np_arr.dtype())) == "int32" );
  CHECK( np_arr.ndim() == 2 );
  CHECK( np_arr.shape()[0] == 2 );
  CHECK( np_arr.shape()[1] == 3 );
  auto np_arr_data = (I4*)np_arr.data();
  CHECK( np_arr_data[0] == 0 );
  CHECK( np_arr_data[1] == 3 );
  CHECK( np_arr_data[2] == 1 );
  CHECK( np_arr_data[3] == 4 );
  CHECK( np_arr_data[4] == 2 );
  CHECK( np_arr_data[5] == 5 );

  // Check the data is shared
  CHECK( np_arr.data() == data(val) );
}
