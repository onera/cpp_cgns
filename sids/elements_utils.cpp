#include "cpp_cgns/sids/elements_utils.hpp"


#include <vector>
#include <algorithm>
#include "cpp_cgns/exception.hpp"
#include "std_e/algorithm/algorithm.hpp"


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

auto to_string(ElementType_t elt) -> std::string {
  return find_by_elt_type(elt)->name;
}


auto element_types_of_dimension(int dim) -> std::vector<ElementType_t> {
  if (dim==0) return {NODE};

  if (dim==1) return {BAR_2, BAR_3, BAR_4, BAR_5};

  if (dim==2) return {
    TRI_3, TRI_6, TRI_9, TRI_10, TRI_12, TRI_15,
    QUAD_4, QUAD_8, QUAD_9, QUAD_12, QUAD_16, QUAD_P4_16, QUAD_25, NGON_n
  }; // note: no MIXED

  if (dim==3) return {
    TETRA_4, TETRA_10, TETRA_16, TETRA_20, TETRA_22, TETRA_34, TETRA_35,
    PYRA_5, PYRA_13, PYRA_14, PYRA_21, PYRA_29, PYRA_30, PYRA_P4_29, PYRA_50, PYRA_55,
    PENTA_6, PENTA_15, PENTA_18, PENTA_24, PENTA_38, PENTA_40, PENTA_33, PENTA_66, PENTA_75,
    HEXA_8, HEXA_20, HEXA_27, HEXA_32, HEXA_56, HEXA_64, HEXA_44, HEXA_98, HEXA_125, NFACE_n
  }; // note: no MIXED

  return {};
}

auto grid_location(ElementType_t elt_type) -> std::string {
  auto elts_dim0 = element_types_of_dimension(0);
  auto elts_dim1 = element_types_of_dimension(1);
  auto elts_dim2 = element_types_of_dimension(2);
  auto elts_dim3 = element_types_of_dimension(3);
  if (std_e::contains(elts_dim0,elt_type)) return "Vertex";
  if (std_e::contains(elts_dim1,elt_type)) return "EdgeCenter";
  if (std_e::contains(elts_dim2,elt_type)) return "FaceCenter";
  if (std_e::contains(elts_dim3,elt_type)) return "CellCenter";
  throw cgns_exception("Unable to find grid location associated to elt \""+to_string(elt_type)+"\"");
}

} // cgns
