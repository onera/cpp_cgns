#include "doctest/doctest.h"
#include "cpp_cgns/cgns.hpp"


using namespace cgns;


TEST_CASE("Hand-written tree construction") {
  std::vector<R8> values = {42.,43.,44};
  I8 data_size = values.size();

  tree t = {
    "MyArray", // name
    {"R8",{data_size},values.data()}, // value
    {}, // children (none here)
    "DataArray_t" // SIDS type
  };

  CHECK( t.name == "MyArray" );

  CHECK( t.value.data_type == "R8" );
  CHECK( t.value.dims == std::vector<I8>{3} );
  CHECK( ((R8*)t.value.data)[0] == 42. );
  CHECK( ((R8*)t.value.data)[1] == 43. );
  CHECK( ((R8*)t.value.data)[2] == 44. );

  CHECK( t.children.size() == 0 );

  CHECK( t.type == "DataArray_t" );




  SUBCASE("Adding a sub-tree as a child") {
    std::vector<I4> sub_values = {2,3,4,5,6,7};
    tree sub_t = {
      "SubArray",
      {"I4",{2,3},sub_values.data()}, // the user is responsible for giving 
                                      // the corrent data_type ("I4") 
                                      // and the correct dimension (here, 2 x 3)
      {},
      "IndexArray_t"
    };
    emplace_child(t,std::move(sub_t));

    CHECK( t.children.size() == 1 );
    CHECK( t.children[0].name == "SubArray" );
  }


}

