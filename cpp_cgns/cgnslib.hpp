#pragma once


/// Extracted from the real "cgnslib.hpp" (because of incompatibility problems). Should be replaced by the real third-party library.


#ifdef CGNS_SCOPE_ENUMS
/* set scope prefix for values only */
#define CGNS_ENUMV( e ) CG_ ## e
/* set scope prefix for types */
#define CGNS_ENUMT( e ) CG_ ## e
/* set scope prefix for defines */
#define CGNS_ENUMD( e ) CG_ ## e
/* set scope prefix for variables (lowercase same as functions) */
#define CGNS_ENUMF( e ) cg_ ## e
#else
#define CGNS_ENUMV( e ) e
#define CGNS_ENUMT( e ) e
#define CGNS_ENUMD( e ) e
#define CGNS_ENUMF( e ) e
#endif


typedef enum {
  CGNS_ENUMV( ElementTypeNull  ) =0,
  CGNS_ENUMV( ElementTypeUserDefined ) =1,
  CGNS_ENUMV( NODE ) =2,
  CGNS_ENUMV( BAR_2 ) =3,
  CGNS_ENUMV( BAR_3 ) =4,
  CGNS_ENUMV( TRI_3 ) =5,
  CGNS_ENUMV( TRI_6 ) =6,
  CGNS_ENUMV( QUAD_4 ) =7,
  CGNS_ENUMV( QUAD_8 ) =8,
  CGNS_ENUMV( QUAD_9 ) =9,
  CGNS_ENUMV( TETRA_4 ) =10,
  CGNS_ENUMV( TETRA_10 ) =11,
  CGNS_ENUMV( PYRA_5 ) =12,
  CGNS_ENUMV( PYRA_14 ) =13,
  CGNS_ENUMV( PENTA_6 ) =14,
  CGNS_ENUMV( PENTA_15 ) =15,
  CGNS_ENUMV( PENTA_18 ) =16,
  CGNS_ENUMV( HEXA_8 ) =17,
  CGNS_ENUMV( HEXA_20 ) =18,
  CGNS_ENUMV( HEXA_27 ) =19,
  CGNS_ENUMV( MIXED ) =20,
  CGNS_ENUMV( PYRA_13 ) =21,
  CGNS_ENUMV( NGON_n ) =22,
  CGNS_ENUMV( NFACE_n ) =23,
  CGNS_ENUMV( BAR_4 ) =24,
  CGNS_ENUMV( TRI_9 ) =25,
  CGNS_ENUMV( TRI_10 ) =26,
  CGNS_ENUMV( QUAD_12 ) =27,
  CGNS_ENUMV( QUAD_16 ) =28,
  CGNS_ENUMV( TETRA_16 ) =29,
  CGNS_ENUMV( TETRA_20 ) =30,
  CGNS_ENUMV( PYRA_21 ) =31,
  CGNS_ENUMV( PYRA_29 ) =32,
  CGNS_ENUMV( PYRA_30 ) =33,
  CGNS_ENUMV( PENTA_24 ) =34,
  CGNS_ENUMV( PENTA_38 ) =35,
  CGNS_ENUMV( PENTA_40 ) =36,
  CGNS_ENUMV( HEXA_32 ) =37,
  CGNS_ENUMV( HEXA_56 ) =38,
  CGNS_ENUMV( HEXA_64 ) =39,
  CGNS_ENUMV( BAR_5 )=40,
  CGNS_ENUMV( TRI_12 )=41,
  CGNS_ENUMV( TRI_15 )=42,
  CGNS_ENUMV( QUAD_P4_16 )=43,
  CGNS_ENUMV( QUAD_25 )=44,
  CGNS_ENUMV( TETRA_22 )=45,
  CGNS_ENUMV( TETRA_34 )=46,
  CGNS_ENUMV( TETRA_35 )=47,
  CGNS_ENUMV( PYRA_P4_29 )=48,
  CGNS_ENUMV( PYRA_50 )=49,
  CGNS_ENUMV( PYRA_55 )=50,
  CGNS_ENUMV( PENTA_33 )=51,
  CGNS_ENUMV( PENTA_66 )=52,
  CGNS_ENUMV( PENTA_75 )=53,
  CGNS_ENUMV( HEXA_44 )=54,
  CGNS_ENUMV( HEXA_98 )=55,
  CGNS_ENUMV( HEXA_125 )=56
} CGNS_ENUMT( ElementType_t );

