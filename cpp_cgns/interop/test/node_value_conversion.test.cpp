#if __cplusplus > 201703L
#include "std_e/unit_test/doctest_pybind.hpp"

#include "cpp_cgns/interop/node_value_conversion.hpp"
using namespace cgns;

PYBIND_TEST_CASE("view_as_node_value") {
  SUBCASE("Numeric types") {
    SUBCASE("I4") {
      std::vector<I4> v = {0,1,2};
      auto np_array = py::array(py::dtype("int32"),3,v.data());

      auto val = view_as_node_value(np_array);
      CHECK( val.data_type() == "I4" );
      CHECK( val.rank() == 1 );
      CHECK( val.extent(0) == 3 );
      CHECK( val(0) == 0 );
      CHECK( val(1) == 1 );
      CHECK( val(2) == 2 );

      CHECK( val.data() == np_array.data() );
    }
    SUBCASE("I8") {
      std::vector<I8> v = {10'000'000'000,1,2};
      auto np_array = py::array(py::dtype("int64"),3,v.data());

      auto val = view_as_node_value(np_array);
      CHECK( val.data_type() == "I8" );
      CHECK( val.rank() == 1 );
      CHECK( val.extent(0) == 3 );
      CHECK( val(0) == 10'000'000'000 );
      CHECK( val(1) == 1 );
      CHECK( val(2) == 2 );

      CHECK( val.data() == np_array.data() );
    }
    SUBCASE("R4") {
      std::vector<R4> v = {0.,1.5,2.5};
      auto np_array = py::array(py::dtype("float32"),3,v.data());

      auto val = view_as_node_value(np_array);
      CHECK( val.data_type() == "R4" );
      CHECK( val.rank() == 1 );
      CHECK( val.extent(0) == 3 );
      CHECK( val(0) == 0. );
      CHECK( val(1) == 1.5 );
      CHECK( val(2) == 2.5 );

      CHECK( val.data() == np_array.data() );
    }
    SUBCASE("R8") {
      std::vector<R8> v = {0.,1.5,2.5};
      auto np_array = py::array(py::dtype("float64"),3,v.data());

      auto val = view_as_node_value(np_array);
      CHECK( val.data_type() == "R8" );
      CHECK( val.rank() == 1 );
      CHECK( val.extent(0) == 3 );
      CHECK( val(0) == 0. );
      CHECK( val(1) == 1.5 );
      CHECK( val(2) == 2.5 );

      CHECK( val.data() == np_array.data() );
    }
    SUBCASE("C1") {
      std::vector<char> v = {'a','b','c'};
      auto np_array = py::array(py::dtype("|S1"),3,v.data());

      auto val = view_as_node_value(np_array);
      CHECK( val.data_type() == "C1" );
      CHECK( val.rank() == 1 );
      CHECK( val.extent(0) == 3 );
      CHECK( val(0) == 'a' );
      CHECK( val(1) == 'b' );
      CHECK( val(2) == 'c' );

      CHECK( val.data() == np_array.data() );
    }
  }

  SUBCASE("Multi-dimensional") {
    SUBCASE("I4") {
      std::vector<I4> v = {0,1,2,3,4,5};
      std::vector<I4> dims = {2,3};
      auto np_array = py::array_t<I4,py::array::f_style>(dims,v.data());

      auto val = view_as_node_value(np_array);
      CHECK( val.data_type() == "I4" );
      CHECK( val.extent() == std::vector<I8>{2,3} );
      CHECK( val(0,0) == 0 ); CHECK( val(0,1) == 2 ); CHECK( val(0,2) == 4 );
      CHECK( val(1,0) == 1 ); CHECK( val(1,1) == 3 ); CHECK( val(1,2) == 5 );

      CHECK( val.data() == np_array.data() );
    }
  }
}

PYBIND_TEST_CASE("to_np_array") {
  node_value val = {{0,1,2},{3,4,5}};
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
  CHECK( np_arr.data() == val.data() );
}
#endif // C++>17
