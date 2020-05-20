#pragma once


#include <string>
#include <vector>
#include "cpp_cgns/cgnslib.h"
#include "cpp_cgns/cgns.hpp"
#include "cpp_cgns/tree_manip.hpp"
#include "std_e/future/contract.hpp"
#include "cpp_cgns/sids/Grid_Coordinates_Elements_and_Flow_Solution.hpp"
#include "std_e/future/make_array.hpp"


namespace cgns {
  

struct element_traits {
  ElementType_t elt_type;
  const char* name;
  int nb_nodes;
};
inline constexpr std::array elements_traits = std_e::make_array(
  element_traits{ ElementTypeNull         , "ElementTypeNull"        ,  0  },
  element_traits{ ElementTypeUserDefined  , "ElementTypeUserDefined" , -1  },
  element_traits{ NODE                    , "NODE"                   ,  1  },
  element_traits{ BAR_2                   , "BAR_2"                  ,  2  },
  element_traits{ BAR_3                   , "BAR_3"                  ,  3  },
  element_traits{ TRI_3                   , "TRI_3"                  ,  3  },
  element_traits{ TRI_6                   , "TRI_6"                  ,  6  },
  element_traits{ QUAD_4                  , "QUAD_4"                 ,  4  },
  element_traits{ QUAD_8                  , "QUAD_8"                 ,  8  },
  element_traits{ QUAD_9                  , "QUAD_9"                 ,  9  },
  element_traits{ TETRA_4                 , "TETRA_4"                ,  4  },
  element_traits{ TETRA_10                , "TETRA_10"               ,  10 },
  element_traits{ PYRA_5                  , "PYRA_5"                 ,  5  },
  element_traits{ PYRA_14                 , "PYRA_14"                ,  14 },
  element_traits{ PENTA_6                 , "PENTA_6"                ,  6  },
  element_traits{ PENTA_15                , "PENTA_15"               ,  15 },
  element_traits{ PENTA_18                , "PENTA_18"               ,  18 },
  element_traits{ HEXA_8                  , "HEXA_8"                 ,  8  },
  element_traits{ HEXA_20                 , "HEXA_20"                ,  20 },
  element_traits{ HEXA_27                 , "HEXA_27"                ,  27 },
  element_traits{ MIXED                   , "MIXED"                  , -1  },
  element_traits{ PYRA_13                 , "PYRA_13"                ,  13 },
  element_traits{ NGON_n                  , "NGON_n"                 , -1  },
  element_traits{ NFACE_n                 , "NFACE_n"                , -1  },
  element_traits{ BAR_4                   , "BAR_4"                  ,  4  },
  element_traits{ TRI_9                   , "TRI_9"                  ,  9  },
  element_traits{ TRI_10                  , "TRI_10"                 ,  10 },
  element_traits{ QUAD_12                 , "QUAD_12"                ,  12 },
  element_traits{ QUAD_16                 , "QUAD_16"                ,  16 },
  element_traits{ TETRA_16                , "TETRA_16"               ,  16 },
  element_traits{ TETRA_20                , "TETRA_20"               ,  20 },
  element_traits{ PYRA_21                 , "PYRA_21"                ,  21 },
  element_traits{ PYRA_29                 , "PYRA_29"                ,  29 },
  element_traits{ PYRA_30                 , "PYRA_30"                ,  30 },
  element_traits{ PENTA_24                , "PENTA_24"               ,  24 },
  element_traits{ PENTA_38                , "PENTA_38"               ,  38 },
  element_traits{ PENTA_40                , "PENTA_40"               ,  40 },
  element_traits{ HEXA_32                 , "HEXA_32"                ,  32 },
  element_traits{ HEXA_56                 , "HEXA_56"                ,  56 },
  element_traits{ HEXA_64                 , "HEXA_64"                ,  64 },
  element_traits{ BAR_5                   , "BAR_5"                  ,  5  },
  element_traits{ TRI_12                  , "TRI_12"                 ,  12 },
  element_traits{ TRI_15                  , "TRI_15"                 ,  15 },
  element_traits{ QUAD_P4_16              , "QUAD_P4_16"             ,  16 },
  element_traits{ QUAD_25                 , "QUAD_25"                ,  25 },
  element_traits{ TETRA_22                , "TETRA_22"               ,  22 },
  element_traits{ TETRA_34                , "TETRA_34"               ,  34 },
  element_traits{ TETRA_35                , "TETRA_35"               ,  35 },
  element_traits{ PYRA_P4_29              , "PYRA_P4_29"             ,  29 },
  element_traits{ PYRA_50                 , "PYRA_50"                ,  50 },
  element_traits{ PYRA_55                 , "PYRA_55"                ,  55 },
  element_traits{ PENTA_33                , "PENTA_33"               ,  33 },
  element_traits{ PENTA_66                , "PENTA_66"               ,  66 },
  element_traits{ PENTA_75                , "PENTA_75"               ,  75 },
  element_traits{ HEXA_44                 , "HEXA_44"                ,  44 },
  element_traits{ HEXA_98                 , "HEXA_98"                ,  98 },
  element_traits{ HEXA_125                , "HEXA_125"               ,  125}
);

using all_basic_2D_elements = std::integer_sequence<ElementType_t,TRI_3,QUAD_4>;
using all_basic_3D_elements = std::integer_sequence<ElementType_t,TETRA_4,PYRA_5,PENTA_6,HEXA_8>;
using all_homogenous_basic_2D_and_3D_elements = std::integer_sequence<ElementType_t,TRI_3,QUAD_4,TETRA_4,PYRA_5,PENTA_6,HEXA_8>;
using all_basic_2D_and_3D_elements = std::integer_sequence<ElementType_t,TRI_3,QUAD_4,TETRA_4,PYRA_5,PENTA_6,HEXA_8,MIXED>;

constexpr int number_of_nodes(int n) { return elements_traits[n].nb_nodes; };

std::string cgns_element_to_string(ElementType_t elt);


std::vector<ElementType_t> element_types_of_dimension(int dim);


} // cgns
