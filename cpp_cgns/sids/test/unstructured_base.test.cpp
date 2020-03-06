#include "doctest/doctest.h"


//#include "cgns_op/sids/example/unstructured_base.hpp"


TEST_CASE("unstructured mesh construction") {
  FAIL("TODO");
  //auto base = create_unstructured_base();

  //// zone 0
  //auto& z0 = get_child_by_type(base,"Zone_t");

  //// sizes
  //CHECK( VertexSize_U(z0) == 24 );
  //CHECK( CellSize_U(z0) == 6 );
  //CHECK( VertexSizeBoundary_U(z0) == 0 );

  //// coordinates
  //tree& coordX_node = get_node_by_matching(z0,"GridCoordinates/CoordinatesX");
  //auto coord_X = view_as_span<R8>(coordX_node);
  //FAIL("TODO");
  ////std::vector<double> expected_z0_coord_X = {
  ////  0.,1.,2.,3.,
  ////  0.,1.,2.,3.,
  ////  0.,1.,2.,3.,
  ////  0.,1.,2.,3.,
  ////  0.,1.,2.,3.,
  ////  0.,1.,2.,3.
  ////};
  ////REQUIRE( z0_coordX.size() == expected_z0_coord_X.size() );
  ////for (size_t i=0; i<expected_z0_coord_X.size(); ++i) {
  ////  CHECK( z0_coordX[i] == doctest::Approx(expected_z0_coord_X[i]) );
  ////}

  ////// bcs
  ////auto& z0_inflow_pl = z0.ZoneBC.BCs[0].PointList.Data.view();
  ////REQUIRE( z0_inflow_pl.size() == 2 );
  ////CHECK( z0_inflow_pl[0] == 1 );
  ////CHECK( z0_inflow_pl[1] == 2 );

  ////// gcs
  ////auto& z0_grid_connec_pl = z0.ZoneGridConnectivity.GridConnectivities[0].PointList.Data.view();
  ////auto& z0_grid_connec_pld = z0.ZoneGridConnectivity.GridConnectivities[0].PointListDonor.Data.view();
  ////REQUIRE( z0_grid_connec_pl.size() == 1 );
  ////CHECK( z0_grid_connec_pl[0] == 7 );
  ////REQUIRE( z0_grid_connec_pld.size() == 1 );
  ////CHECK( z0_grid_connec_pld[0] == 1 );

  ////// elements
  ////auto& z0_ngon = z0.ZoneElements[0];
  ////auto& z0_ngon_elt_range = z0_ngon.ElementRange;
  ////CHECK( z0_ngon_elt_range.Begin[0] == 1 );
  ////CHECK( z0_ngon_elt_range.End[0] == 8 + 9 + 12 );
  ////auto& z0_ngon_elt_connect = z0_ngon.ElementConnectivity.Data.view();
  ////REQUIRE( z0_ngon_elt_connect.size() == (8 + 9 + 12)*(1+4) );
  ////CHECK( z0_ngon_elt_connect[0] ==  4 );
  ////CHECK( z0_ngon_elt_connect[1] ==  1 );
  ////CHECK( z0_ngon_elt_connect[2] ==  5 );
  ////CHECK( z0_ngon_elt_connect[3] == 17 );
  ////CHECK( z0_ngon_elt_connect[4] == 13 );
  ////auto& z0_ngon_parent_elts = z0_ngon.ParentElements.Data.view();
  ////REQUIRE( z0_ngon_parent_elts.size() == (8 + 9 + 12)*2 );
  ////CHECK( z0_ngon_parent_elts({0,0}) == 0 ); CHECK( z0_ngon_parent_elts({0,1}) == 1 );
  ////CHECK( z0_ngon_parent_elts({1,0}) == 0 ); CHECK( z0_ngon_parent_elts({1,1}) == 4 );
  ////CHECK( z0_ngon_parent_elts({2,0}) == 1 ); CHECK( z0_ngon_parent_elts({2,1}) == 2 );
  ////CHECK( z0_ngon_parent_elts({3,0}) == 4 ); CHECK( z0_ngon_parent_elts({3,1}) == 5 );
}
