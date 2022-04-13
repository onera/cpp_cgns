#if __cplusplus > 201703L
#include "std_e/unit_test/doctest.hpp"

#include "cpp_cgns/sids/Grid_Coordinates_Elements_and_Flow_Solution.hpp"
#include "cpp_cgns/sids/creation.hpp"

using namespace cgns;

TEST_CASE("element sections") {
  tree quads = new_Elements("Quads", QUAD_4, std::vector{1,2,3,4, 4,3,7,8}, 1, 2);

  CHECK( ElementType(quads) == I4(QUAD_4) );
  CHECK( ElementSizeBoundary(quads) == 0 );

  tree z = {"Zone","Zone_t",MT(),{std::move(quads)}};
  tree& quads_ref = element_section(z,QUAD_4);
  CHECK( name(quads_ref) == "Quads" );
}
#endif // C++>17
