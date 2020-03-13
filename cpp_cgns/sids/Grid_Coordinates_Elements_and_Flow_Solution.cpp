#include "cpp_cgns/sids/Grid_Coordinates_Elements_and_Flow_Solution.hpp"
#include "std_e/future/contract.hpp"
#include "cpp_cgns/cgns_exception.hpp"


namespace cpp_cgns {

std_e::span<const I4,2> ElementType_ElementSizeBoundary(const tree& e) {
  STD_E_ASSERT(e.type=="Elements_t");
  if (e.value.data_type!="I4")
    throw cgns_exception("CGNS requires zone dimensions to be of type \"I4\""); // TODO I8 (and ssqq)
  const I4* e_ptr = (const I4*)e.value.data;
  return std_e::make_span<2>(e_ptr);
}
std_e::span<I4,2> ElementType_ElementSizeBoundary(tree& e) {
  STD_E_ASSERT(e.type=="Elements_t");
  if (e.value.data_type!="I4")
    throw cgns_exception("CGNS requires zone dimensions to be of type \"I4\""); // TODO I8 (and ssqq)
  I4* e_ptr = (I4*)e.value.data;
  return std_e::make_span<2>(e_ptr);
}

I4 ElementType(const tree& e) {
  STD_E_ASSERT(e.type=="Elements_t");
  return ElementType_ElementSizeBoundary(e)[0];
}
I4& ElementType(tree& e) {
  STD_E_ASSERT(e.type=="Elements_t");
  return ElementType_ElementSizeBoundary(e)[0];
}

I4 ElementSizeBoundary(const tree& e) {
  STD_E_ASSERT(e.type=="Elements_t");
  return ElementType_ElementSizeBoundary(e)[1];
}
I4& ElementSizeBoundary(tree& e) {
  STD_E_ASSERT(e.type=="Elements_t");
  return ElementType_ElementSizeBoundary(e)[1];
}

bool is_boundary_partitionned_element_pool(const tree& e) {
  STD_E_ASSERT(e.type=="Elements_t");
  return ElementSizeBoundary(e) != 0; // per CGNS SIDS 7.3
}

} // cpp_cgns
