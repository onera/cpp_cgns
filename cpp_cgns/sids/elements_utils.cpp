#if __cplusplus > 201703L
#include "cpp_cgns/sids/elements_utils.hpp"


#include <vector>
#include <algorithm>
#include "std_e/algorithm/algorithm.hpp"
#include "cpp_cgns/base/exception.hpp"


namespace cgns {


auto find_by_elt_type(ElementType_t elt) {
  auto match_cgns_elt = [elt](const auto& cgns_nb_nodes_elt_pair){ return cgns_nb_nodes_elt_pair.elt_type==elt; };
  auto pos = std::find_if(begin(elements_traits),end(elements_traits),match_cgns_elt);
  if (pos==end(elements_traits)) {
    throw cgns_exception(std::string("Unknown CGNS element type for number of nodes\"")+to_string(elt)+"\"");
  } else {
    return pos;
  }
}

auto grid_location(ElementType_t elt_type) -> std::string {
  auto elt_dim = element_dimension(elt_type);
  if (elt_dim==0) return "Vertex";
  if (elt_dim==1) return "EdgeCenter";
  if (elt_dim==2) return "FaceCenter";
  if (elt_dim==3) return "CellCenter";
  throw cgns_exception("Unable to find grid location associated to elt \""+to_string(elt_type)+"\"");
}

} // cgns
#endif // C++>17
