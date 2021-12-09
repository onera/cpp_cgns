#pragma once


#include <string>
#include <vector>
#include "cpp_cgns/sids/cgnslib.h"
#include "std_e/future/contract.hpp"
#include "std_e/future/make_array.hpp"
#include "std_e/future/constexpr_vector.hpp"


namespace cgns {


template<class T> using small_array = std_e::constexpr_vector<T,8>;


struct element_traits {
  ElementType_t elt_type;
  int n_vtx;
  int dimension;
  small_array<ElementType_t> face_types;
  small_array<int> n_face_by_type;
};
inline constexpr std::array elements_traits = std_e::make_array(
  //                                              n_vtx , dim,     face_types   ,n_face_by_type
  element_traits{ /*00*/ ElementTypeNull         ,  0   , -1 , {}               , {}    },
  element_traits{ /*01*/ ElementTypeUserDefined  , -1   , -1 , {ElementTypeNull}, {-1}  },
  element_traits{ /*02*/ NODE                    ,  1   ,  0 , {}               , {}    },
  element_traits{ /*03*/ BAR_2                   ,  2   ,  1 , {}               , {}    },
  element_traits{ /*04*/ BAR_3                   ,  3   ,  1 , {}               , {}    },
  element_traits{ /*05*/ TRI_3                   ,  3   ,  2 , {TRI_3}          , {1}   },
  element_traits{ /*06*/ TRI_6                   ,  6   ,  2 , {TRI_6}          , {1}   },
  element_traits{ /*07*/ QUAD_4                  ,  4   ,  2 , {QUAD_4}         , {1}   },
  element_traits{ /*08*/ QUAD_8                  ,  8   ,  2 , {QUAD_8}         , {1}   },
  element_traits{ /*09*/ QUAD_9                  ,  9   ,  2 , {QUAD_9}         , {1}   },
  element_traits{ /*10*/ TETRA_4                 ,  4   ,  3 , {TRI_3}          , {4}   },
  element_traits{ /*11*/ TETRA_10                ,  10  ,  3 , {TRI_6}          , {4}   },
  element_traits{ /*12*/ PYRA_5                  ,  5   ,  3 , {TRI_3,QUAD_4}   , {4,1} },
  element_traits{ /*13*/ PYRA_14                 ,  14  ,  3 , {TRI_6,QUAD_9}   , {4,1} },
  element_traits{ /*14*/ PENTA_6                 ,  6   ,  3 , {TRI_3,QUAD_4}   , {2,3} },
  element_traits{ /*15*/ PENTA_15                ,  15  ,  3 , {TRI_6,QUAD_8}   , {2,3} },
  element_traits{ /*16*/ PENTA_18                ,  18  ,  3 , {TRI_6,QUAD_9}   , {2,3} },
  element_traits{ /*17*/ HEXA_8                  ,  8   ,  3 , {QUAD_4}         , {6}   },
  element_traits{ /*18*/ HEXA_20                 ,  20  ,  3 , {QUAD_8}         , {6}   },
  element_traits{ /*19*/ HEXA_27                 ,  27  ,  3 , {QUAD_9}         , {6}   },
  element_traits{ /*20*/ MIXED                   , -1   , -1 , {ElementTypeNull}, {-1}  },
  element_traits{ /*21*/ PYRA_13                 ,  13  ,  3 , {TRI_6,QUAD_8}   , {4,1}  },
  element_traits{ /*22*/ NGON_n                  , -1   ,  2 , {ElementTypeNull}, {-1}  },
  element_traits{ /*23*/ NFACE_n                 , -1   ,  3 , {ElementTypeNull}, {-1}  },
  element_traits{ /*24*/ BAR_4                   ,  4   ,  1 , {}               , {} }, // TODO: fill face info from here
  element_traits{ /*25*/ TRI_9                   ,  9   ,  2 , {}               , {} },
  element_traits{ /*26*/ TRI_10                  ,  10  ,  2 , {}               , {} },
  element_traits{ /*27*/ QUAD_12                 ,  12  ,  2 , {}               , {} },
  element_traits{ /*28*/ QUAD_16                 ,  16  ,  2 , {}               , {} },
  element_traits{ /*29*/ TETRA_16                ,  16  ,  3 , {}               , {} },
  element_traits{ /*30*/ TETRA_20                ,  20  ,  3 , {}               , {} },
  element_traits{ /*31*/ PYRA_21                 ,  21  ,  3 , {}               , {} },
  element_traits{ /*32*/ PYRA_29                 ,  29  ,  3 , {}               , {} },
  element_traits{ /*33*/ PYRA_30                 ,  30  ,  3 , {}               , {} },
  element_traits{ /*34*/ PENTA_24                ,  24  ,  3 , {}               , {} },
  element_traits{ /*35*/ PENTA_38                ,  38  ,  3 , {}               , {} },
  element_traits{ /*36*/ PENTA_40                ,  40  ,  3 , {}               , {} },
  element_traits{ /*37*/ HEXA_32                 ,  32  ,  3 , {}               , {} },
  element_traits{ /*38*/ HEXA_56                 ,  56  ,  3 , {}               , {} },
  element_traits{ /*39*/ HEXA_64                 ,  64  ,  3 , {}               , {} },
  element_traits{ /*40*/ BAR_5                   ,  5   ,  1 , {}               , {} },
  element_traits{ /*41*/ TRI_12                  ,  12  ,  2 , {}               , {} },
  element_traits{ /*42*/ TRI_15                  ,  15  ,  2 , {}               , {} },
  element_traits{ /*43*/ QUAD_P4_16              ,  16  ,  2 , {}               , {} },
  element_traits{ /*44*/ QUAD_25                 ,  25  ,  2 , {}               , {} },
  element_traits{ /*45*/ TETRA_22                ,  22  ,  3 , {}               , {} },
  element_traits{ /*46*/ TETRA_34                ,  34  ,  3 , {}               , {} },
  element_traits{ /*47*/ TETRA_35                ,  35  ,  3 , {}               , {} },
  element_traits{ /*48*/ PYRA_P4_29              ,  29  ,  3 , {}               , {} },
  element_traits{ /*49*/ PYRA_50                 ,  50  ,  3 , {}               , {} },
  element_traits{ /*50*/ PYRA_55                 ,  55  ,  3 , {}               , {} },
  element_traits{ /*51*/ PENTA_33                ,  33  ,  3 , {}               , {} },
  element_traits{ /*52*/ PENTA_66                ,  66  ,  3 , {}               , {} },
  element_traits{ /*53*/ PENTA_75                ,  75  ,  3 , {}               , {} },
  element_traits{ /*54*/ HEXA_44                 ,  44  ,  3 , {}               , {} },
  element_traits{ /*55*/ HEXA_98                 ,  98  ,  3 , {}               , {} },
  element_traits{ /*56*/ HEXA_125                ,  125 ,  3 , {}               , {} }
);

using all_basic_2D_elements = std::integer_sequence<ElementType_t,TRI_3,QUAD_4>;
using all_basic_3D_elements = std::integer_sequence<ElementType_t,TETRA_4,PYRA_5,PENTA_6,HEXA_8>;
using all_homogenous_basic_2D_and_3D_elements = std::integer_sequence<ElementType_t,TRI_3,QUAD_4,TETRA_4,PYRA_5,PENTA_6,HEXA_8>;
using all_basic_2D_and_3D_elements = std::integer_sequence<ElementType_t,TRI_3,QUAD_4,TETRA_4,PYRA_5,PENTA_6,HEXA_8,MIXED>;

constexpr auto number_of_vertices     (ElementType_t elt_type) -> int   { return elements_traits[elt_type].n_vtx; };
constexpr auto element_dimension      (ElementType_t elt_type) -> int   { return elements_traits[elt_type].dimension; }
constexpr auto face_types             (ElementType_t elt_type) -> auto& { return elements_traits[elt_type].face_types; }
constexpr auto number_of_faces_by_type(ElementType_t elt_type) -> auto& { return elements_traits[elt_type].n_face_by_type; }

auto grid_location(ElementType_t elt_type) -> std::string;

} // cgns
