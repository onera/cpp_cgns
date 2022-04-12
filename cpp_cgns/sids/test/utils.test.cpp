#if __cplusplus > 201703L
#include "std_e/unit_test/doctest.hpp"

#include "cpp_cgns/base/md_array.hpp"
#include "cpp_cgns/sids/utils.hpp"

using namespace cgns;

TEST_CASE("sids_utils is_boundary") {
  md_array<I4,2> parent_elts =
    { {10, 11,  0,  0},
      {12,  0, 13,  0} };
    //  ^   ^   ^   ^
    //  0   1   2   3
  auto pe_view = std_e::make_view(parent_elts);

  CHECK_FALSE(is_boundary(pe_view,0));
  CHECK      (is_boundary(pe_view,1));
  CHECK      (is_boundary(pe_view,2));
  CHECK      (is_boundary(pe_view,3));
}
#endif // C++>17
