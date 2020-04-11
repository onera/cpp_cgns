#include "cpp_cgns/sids/elements_utils.hpp"


#include <vector>
#include <algorithm>
#include "cpp_cgns/exception.hpp"


namespace cgns {


auto find_by_elt_type(ElementType_t elt) {
  auto match_cgns_elt = [elt](const auto& cgns_nb_nodes_elt_pair){ return cgns_nb_nodes_elt_pair.elt_type==elt; };
  auto pos = std::find_if(begin(elements_traits),end(elements_traits),match_cgns_elt);
  if (pos==end(elements_traits)) {
    throw cgns_exception(std::string("Unknown CGNS element type for number of nodes\"")+cgns_element_to_string(elt)+"\"");
  } else {
    return pos;
  }
}

int number_of_nodes(ElementType_t elt) {
  int nb_nodes = find_by_elt_type(elt)->nb_nodes;
  if (nb_nodes==0) {
    throw cgns_exception(std::string("Number of nodes for element of type\"")+cgns_element_to_string(elt)+"\"is unknown or variable");
  } else {
    return nb_nodes;
  }
}

std::string cgns_element_to_string(ElementType_t elt) {
  return find_by_elt_type(elt)->name;
}


// TODO new elements (not only till HEXA_64)
std::vector<ElementType_t> element_types_of_dimension(int dim) {
  if (dim==0) return {NODE};

  if (dim==1) return {BAR_2, BAR_3, BAR_4};

  if (dim==2) return {TRI_3, TRI_6, TRI_9, TRI_10, QUAD_4, QUAD_8, QUAD_9, QUAD_12, QUAD_16, NGON_n}; // note: no MIXED

  if (dim==3) return {
    TETRA_4, TETRA_10, TETRA_16, TETRA_20, PYRA_5, PYRA_13, PYRA_14, PYRA_21,
    PYRA_29, PYRA_30, PENTA_6, PENTA_15, PENTA_18, PENTA_24, PENTA_38, PENTA_40,
    HEXA_8, HEXA_20, HEXA_27, HEXA_32, HEXA_56, HEXA_64, NFACE_n
  }; // note: no MIXED

  return {};
}


} // cgns
