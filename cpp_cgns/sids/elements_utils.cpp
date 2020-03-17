#include "cpp_cgns/sids/elements_utils.hpp"


#include <vector>
#include <algorithm>
#include "cpp_cgns/exception.hpp"


namespace cpp_cgns {


struct element_traits {
  CGNS_ENUMT(ElementType_t) elt_type;
  std::string name;
  int nb_of_nodes;
};
const std::vector<element_traits> elements_traits = {
  { CGNS_ENUMV( ElementTypeNull  )       , "ElementTypeNull"        ,  0  },
  { CGNS_ENUMV( ElementTypeUserDefined ) , "ElementTypeUserDefined" , -1  },
  { CGNS_ENUMV( NODE )                   , "NODE"                   ,  1  },
  { CGNS_ENUMV( BAR_2 )                  , "BAR_2"                  ,  2  },
  { CGNS_ENUMV( BAR_3 )                  , "BAR_3"                  ,  3  },
  { CGNS_ENUMV( TRI_3 )                  , "TRI_3"                  ,  3  },
  { CGNS_ENUMV( TRI_6 )                  , "TRI_6"                  ,  6  },
  { CGNS_ENUMV( QUAD_4 )                 , "QUAD_4"                 ,  4  },
  { CGNS_ENUMV( QUAD_8 )                 , "QUAD_8"                 ,  8  },
  { CGNS_ENUMV( QUAD_9 )                 , "QUAD_9"                 ,  9  },
  { CGNS_ENUMV( TETRA_4 )                , "TETRA_4"                ,  4  },
  { CGNS_ENUMV( TETRA_10 )               , "TETRA_10"               ,  10 },
  { CGNS_ENUMV( PYRA_5 )                 , "PYRA_5"                 ,  5  },
  { CGNS_ENUMV( PYRA_14 )                , "PYRA_14"                ,  14 },
  { CGNS_ENUMV( PENTA_6 )                , "PENTA_6"                ,  6  },
  { CGNS_ENUMV( PENTA_15 )               , "PENTA_15"               ,  15 },
  { CGNS_ENUMV( PENTA_18 )               , "PENTA_18"               ,  18 },
  { CGNS_ENUMV( HEXA_8 )                 , "HEXA_8"                 ,  8  },
  { CGNS_ENUMV( HEXA_20 )                , "HEXA_20"                ,  20 },
  { CGNS_ENUMV( HEXA_27 )                , "HEXA_27"                ,  27 },
  { CGNS_ENUMV( MIXED )                  , "MIXED"                  , -1  },
  { CGNS_ENUMV( PYRA_13 )                , "PYRA_13"                ,  13 },
  { CGNS_ENUMV( NGON_n )                 , "NGON_n"                 , -1  },
  { CGNS_ENUMV( NFACE_n )                , "NFACE_n"                , -1  },
  { CGNS_ENUMV( BAR_4 )                  , "BAR_4"                  ,  4  },
  { CGNS_ENUMV( TRI_9 )                  , "TRI_9"                  ,  9  },
  { CGNS_ENUMV( TRI_10 )                 , "TRI_10"                 ,  10 },
  { CGNS_ENUMV( QUAD_12 )                , "QUAD_12"                ,  12 },
  { CGNS_ENUMV( QUAD_16 )                , "QUAD_16"                ,  16 },
  { CGNS_ENUMV( TETRA_16 )               , "TETRA_16"               ,  16 },
  { CGNS_ENUMV( TETRA_20 )               , "TETRA_20"               ,  20 },
  { CGNS_ENUMV( PYRA_21 )                , "PYRA_21"                ,  21 },
  { CGNS_ENUMV( PYRA_29 )                , "PYRA_29"                ,  29 },
  { CGNS_ENUMV( PYRA_30 )                , "PYRA_30"                ,  30 },
  { CGNS_ENUMV( PENTA_24 )               , "PENTA_24"               ,  24 },
  { CGNS_ENUMV( PENTA_38 )               , "PENTA_38"               ,  38 },
  { CGNS_ENUMV( PENTA_40 )               , "PENTA_40"               ,  40 },
  { CGNS_ENUMV( HEXA_32 )                , "HEXA_32"                ,  32 },
  { CGNS_ENUMV( HEXA_56 )                , "HEXA_56"                ,  56 },
  { CGNS_ENUMV( HEXA_64 )                , "HEXA_64"                ,  64 },
  { CGNS_ENUMV( BAR_5 )                  , "BAR_5"                  ,  5  },
  { CGNS_ENUMV( TRI_12 )                 , "TRI_12"                 ,  12 },
  { CGNS_ENUMV( TRI_15 )                 , "TRI_15"                 ,  15 },
  { CGNS_ENUMV( QUAD_P4_16 )             , "QUAD_P4_16"             ,  16 },
  { CGNS_ENUMV( QUAD_25 )                , "QUAD_25"                ,  25 },
  { CGNS_ENUMV( TETRA_22 )               , "TETRA_22"               ,  22 },
  { CGNS_ENUMV( TETRA_34 )               , "TETRA_34"               ,  34 },
  { CGNS_ENUMV( TETRA_35 )               , "TETRA_35"               ,  35 },
  { CGNS_ENUMV( PYRA_P4_29 )             , "PYRA_P4_29"             ,  29 },
  { CGNS_ENUMV( PYRA_50 )                , "PYRA_50"                ,  50 },
  { CGNS_ENUMV( PYRA_55 )                , "PYRA_55"                ,  55 },
  { CGNS_ENUMV( PENTA_33 )               , "PENTA_33"               ,  33 },
  { CGNS_ENUMV( PENTA_66 )               , "PENTA_66"               ,  66 },
  { CGNS_ENUMV( PENTA_75 )               , "PENTA_75"               ,  75 },
  { CGNS_ENUMV( HEXA_44 )                , "HEXA_44"                ,  44 },
  { CGNS_ENUMV( HEXA_98 )                , "HEXA_98"                ,  98 },
  { CGNS_ENUMV( HEXA_125 )               , "HEXA_125"               ,  125}
};


auto find_by_elt_type(CGNS_ENUMT(ElementType_t) elt) {
  auto match_cgns_elt = [elt](const auto& cgns_nb_nodes_elt_pair){ return cgns_nb_nodes_elt_pair.elt_type==elt; };
  auto pos = std::find_if(begin(elements_traits),end(elements_traits),match_cgns_elt);
  if (pos==end(elements_traits)) {
    throw cgns_exception(std::string("Unknown CGNS element type for number of nodes\"")+cgns_element_to_string(elt)+"\"");
  } else {
    return pos;
  }
}

int number_of_nodes(CGNS_ENUMT(ElementType_t) elt) {
  int nb_of_nodes = find_by_elt_type(elt)->nb_of_nodes;
  if (nb_of_nodes==0) {
    throw cgns_exception(std::string("Number of nodes for element of type\"")+cgns_element_to_string(elt)+"\"is unknown or variable");
  } else {
    return nb_of_nodes;
  }
}

std::string cgns_element_to_string(CGNS_ENUMT(ElementType_t) elt) {
  return find_by_elt_type(elt)->name;
}


} // cpp_cgns
