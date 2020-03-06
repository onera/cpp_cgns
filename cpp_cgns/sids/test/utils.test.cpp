#include "doctest/doctest.h"

#include "cpp_cgns/sids/utils.hpp"

using namespace cpp_cgns;

TEST_CASE("sids_utils is_boundary") {
  std_e::dyn_multi_array<I4,int32_t,2> x = {
    {10, 11,  0, 0},
    {12,  0, 13, 0}
  };
  // ^   ^   ^   ^
  // |   |   |   |
  // 0   1   2   3
  node_value parent_elts = view_as_node_value(x);

  CHECK_FALSE(is_boundary(parent_elts,0));
  CHECK      (is_boundary(parent_elts,1));
  CHECK      (is_boundary(parent_elts,2));
  CHECK      (is_boundary(parent_elts,3));
}
