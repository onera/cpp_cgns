#include "cpp_cgns/sids/example/unstructured_base.hpp"
//#include "cgns_op/mesh_op/generate_connectivities.hpp"
//#include "cgns_op/mesh_op/convert_to_ngon.hpp"
//#include "cgns_op/mesh_op/utility.hpp"
using namespace cpp_cgns;

auto
create_GridCoords0(Internal& I) -> tree {
  auto coord_X = make_cgns_vector(
    { 0.,1.,2.,3.,
      0.,1.,2.,3.,
      0.,1.,2.,3.,
      0.,1.,2.,3.,
      0.,1.,2.,3.,
      0.,1.,2.,3. }
    ,I.alloc()
  );
  auto coord_Y = make_cgns_vector(
    { 0.,0.,0.,0.,
      1.,1.,1.,1.,
      2.,2.,2.,2.,
      0.,0.,0.,0.,
      1.,1.,1.,1.,
      2.,2.,2.,2. }
    ,I.alloc()
  );
  auto coord_Z = make_cgns_vector(
    { 0.,0.,0.,0.,
      0.,0.,0.,0.,
      0.,0.,0.,0.,
      1.,1.,1.,1.,
      1.,1.,1.,1.,
      1.,1.,1.,1. }
    ,I.alloc()
  );
  tree grid_coords = I.newGridCoordinates();
  emplace_child(grid_coords,I.newDataArray("CoordinateX",view_as_node_value(coord_X)));
  emplace_child(grid_coords,I.newDataArray("CoordinateY",view_as_node_value(coord_Y)));
  emplace_child(grid_coords,I.newDataArray("CoordinateZ",view_as_node_value(coord_Z)));
  return grid_coords;
}
auto
create_GridCoords1(Internal& I) -> tree {
  auto coord_X = make_cgns_vector(
    { 3.,4.,
      3.,4.,
      3.,4.,
      3.,4. }
    ,I.alloc()
  );
  auto coord_Y = make_cgns_vector(
    { 0.,0.,
      1.,1.,
      0.,0.,
      1.,1. }
    ,I.alloc()
  );
  auto coord_Z = make_cgns_vector(
    { 0.,0.,
      0.,0.,
      1.,1.,
      1.,1. }
    ,I.alloc()
  );
  tree grid_coords = I.newGridCoordinates();
  emplace_child(grid_coords,I.newDataArray("CoordinateX",view_as_node_value(coord_X)));
  emplace_child(grid_coords,I.newDataArray("CoordinateY",view_as_node_value(coord_Y)));
  emplace_child(grid_coords,I.newDataArray("CoordinateZ",view_as_node_value(coord_Z)));
  return grid_coords;
}

auto
create_Zone0(Internal& I) -> tree {
/* Le maillage utilisé est "six quads", cf. simple_meshes.txt */
  int32_t VertexSize = 24;
  int32_t CellSize = 6;
  int32_t VertexSizeBoundary = 0;
  tree zone = I.newUnstructuredZone("Zone0",{VertexSize,CellSize,VertexSizeBoundary});

  emplace_child(zone,create_GridCoords0(I));

  auto pl_bc = make_cgns_vector<I4>(
    {1,2}, // 1,2 are the two i-faces at x=0
    I.alloc()
  );
  tree zone_bc = I.newZoneBC();
  emplace_child(zone_bc,I.newBC("Inlet","FaceCenter",pl_bc));

  auto pl_gc = make_cgns_vector<I4>(
    {7}, // 7 is the bottom i-face at x=3
    I.alloc()
  );
  auto pl_gc_opp = make_cgns_vector<I4>(
    {1,2}, // cf. zone 1
    I.alloc()
  );
  tree gc = I.newGridConnectivity("MixingPlane","Zone1","FaceCenter","Abutting1to1");
  emplace_child(gc,I.newPointList("PointList",pl_gc));
  emplace_child(gc,I.newPointList("PointListDonor",pl_gc_opp));
  tree zone_gc = I.newZoneGridConnectivity();
  emplace_child(zone_gc,std::move(gc));


  //std::array<int32_t,3> nb_vertices = {{4,3,2}};
  //auto quad_faces = generate_faces(nb_vertices);
  //quad_faces = shift_vertices_ids(quad_faces,1); // CGNS is 1-indexed
  //cgns_vector<int32_t> ngons = convert_to_ngon(quad_faces);

  //index_t nb_i_faces = 8;
  //index_t nb_j_faces = 9;
  //index_t nb_k_faces = 12;
  //int32_t nb_ngons = nb_i_faces + nb_j_faces + nb_k_faces;


  //cgns_vector<int32_t> i_faces_l_parent_elements = generate_faces_left_parent_cell_ids(nb_vertices,0);
  //cgns_vector<int32_t> i_faces_r_parent_elements = generate_faces_right_parent_cell_ids(nb_vertices,0);
  //cgns_vector<int32_t> j_faces_l_parent_elements = generate_faces_left_parent_cell_ids(nb_vertices,1);
  //cgns_vector<int32_t> j_faces_r_parent_elements = generate_faces_right_parent_cell_ids(nb_vertices,1);
  //// k-faces are considered interior (only for the sake of having interior nodes),
  //// their parent is imaginary cell #42
  //cgns_vector<int32_t> k_faces_l_parent_elements(nb_k_faces,42);
  //cgns_vector<int32_t> k_faces_r_parent_elements(nb_k_faces,42);

  //cgns_vector<int32_t> parent_elements
  //        = std_e::concatenate(  i_faces_l_parent_elements
  //        , std_e::concatenate(  j_faces_l_parent_elements
  //        , std_e::concatenate(  k_faces_l_parent_elements
  //        , std_e::concatenate(  i_faces_r_parent_elements
  //        , std_e::concatenate(  j_faces_r_parent_elements
  //        ,                      k_faces_r_parent_elements  )))));
  //std_e::shift(parent_elements,1); // cgns indexing begin at 1
  //
  //Elements_t ngon_elements = {
  //    "Ngons",
  //    NGON_n,
  //    ElementRange_t(1,nb_ngons),
  //    ElementConnectivity_t(std::move(ngons)),
  //    ParentElements_t(std::move(parent_elements))
  //};
  //emplace_child(zone,std::move(ngon_elements));

  return zone;
}

auto
create_Zone1(Internal& I) -> tree {
/* Le maillage utilisé est "one quad", cf. simple_meshes.h */
  int32_t VertexSize = 8;
  int32_t CellSize = 1;
  int32_t VertexSizeBoundary = 0;
  tree zone = I.newUnstructuredZone("Zone1",{VertexSize,CellSize,VertexSizeBoundary});

  emplace_child(zone,create_GridCoords1(I));

  //BC_t bc_outlet = {"Outlet","BCOutflowSubsonic",PointList_t<int32_t>({2})};
  //ZoneBC_t ZoneBC = {
  //    { std::move(bc_outlet) } // 2 is the i-face at x=4.
  //};


  //auto pl = PointList_t<int32_t>({1}); // 1 is the i-face at x=3.
  //auto pld = PointListDonor_t<int32_t>({7}); // cf. zone 0
  //GridConnectivity_t gc = {"MixingPlane","Abutting1to1","Zone0",pl,pld};
  //ZoneGridConnectivity_t ZoneGridConnectivity = {
  //    { std::move(gc) }
  //};



  //std::array<int32_t,3> nb_vertices = {{2,2,2}};
  //auto quad_faces = generate_faces(nb_vertices);
  //quad_faces = shift_vertices_ids(quad_faces,1); // CGNS is 1-indexed
  //cgns_vector<int32_t> ngons = convert_to_ngon(quad_faces);

  //int32_t nb_ngons = 2 + 2 + 2;

  //cgns_vector<int32_t> parent_elements = std_e::concatenate(
  //    generate_faces_left_parent_cell_ids(nb_vertices),
  //    generate_faces_right_parent_cell_ids(nb_vertices)
  //);
  //
  //Elements_t ngon_elements = {
  //    "Ngons",
  //    NGON_n,
  //    ElementRange_t(1,nb_ngons),
  //    ElementConnectivity_t(std::move(ngons)),
  //    ParentElements_t(std::move(parent_elements))
  //};
  //emplace_child(zone,std::move(ngon_elements));

  return zone;
}

auto
create_unstructured_base(Internal& I) -> cpp_cgns::tree {
  tree b = I.newCGNSBase("Base0",3,3);
  emplace_child(b,create_Zone0(I));
  emplace_child(b,create_Zone1(I));
  return b;
}

