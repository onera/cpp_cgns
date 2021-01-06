#include "cpp_cgns/sids/creation.hpp"
#include "cpp_cgns/tree_manip.hpp"
#include <functional>
#include "std_e/multi_array/utils.hpp"
#include "cpp_cgns/exception.hpp"
#include "cpp_cgns/sids/cgnslib.h"


namespace cgns {


// factory {

/// node creation {
tree factory::newDataArray(const std::string& name, node_value value) const {
  return {name, "DataArray_t", value, {}};
}
tree factory::newUserDefinedData(const std::string& name, node_value value) const {
  return {name, "UserDefinedData_t", value, {}};
}
tree factory::newUserDefinedData(const std::string& name, const std::string& val) const {
  node_value value = create_string_node_value(val,alloc());
  return {name, "UserDefinedData_t", value, {}};
}

tree factory::newRootNode() const {
  return {"CGNSTree", "CGNSTree_t", MT, {}};
}

tree factory::newCGNSVersionNode() const {
  R4 version = 3.1;
  node_value version_arr = create_node_value_1d({version},alloc());
  return {"CGNSLibraryVersion", "CGNSLibraryVersion_t", version_arr, {}};
}

tree factory::newCGNSTree() const {
  tree t = newRootNode();
  t.children.push_back(newCGNSVersionNode());
  return t;
}

tree factory::newCGNSBase(const std::string& name, I4 cellDim, I4 physDim) const {
  I4* ptr = allocate<I4>(alloc(),2);
  ptr[0] = cellDim;
  ptr[1] = physDim;
  node_value value = {to_string<I4>(),{2},ptr};

  return {name, "CGNSBase_t", value, {}};
}

tree factory::newUnstructuredZone(const std::string& name, const std::array<I4,3>& dims) const {
  node_value z_type_value = create_string_node_value("Unstructured",alloc());
  tree z_type = {"ZoneType", "ZoneType_t", z_type_value, {}};

  node_value dim_value = create_node_value_1d(dims,alloc());
  dim_value.dims = {1,dim_value.dims[0]}; // required by SIDS file mapping (Zone_t)
  return {name, "Zone_t", dim_value, {z_type}};
}

tree factory::newZoneBC() const {
  return {"ZoneBC", "ZoneBC_t", MT, {}};
}
tree factory::newZoneGridConnectivity() const {
  return {"ZoneGridConnectivity", "ZoneGridConnectivity_t", MT, {}};
}

tree factory::newGridCoordinates(const std::string& name) const {
  return {name, "GridCoordinates_t", MT, {}};
}


tree factory::newPointRange(I4 first, I4 last) const {
  node_value range_value = create_node_value_1d({first,last},alloc());
  range_value.dims = {1,2};
  return {"PointRange", "IndexRange_t", range_value, {}};
}
tree factory::newElementRange(I4 first, I4 last) const {
  node_value range_value = create_node_value_1d({first,last},alloc());
  return {"ElementRange", "IndexRange_t", range_value, {}};
}


tree factory::newElements(
  const std::string& name, I4 type, std_e::span<I4> conns,
  I4 first, I4 last, I4 nb_bnd_elts) const
{
  tree elts_conn_node = newDataArray("ElementConnectivity", view_as_node_value(conns));
  tree eltsRange = newElementRange(first,last);
  node_value elts_node_val = create_node_value_1d({type,nb_bnd_elts},alloc());
  return {name, "Elements_t", elts_node_val, {eltsRange,elts_conn_node}};
}
tree factory::newHomogenousElements(
  const std::string& name, I4 type, md_array_view<I4,2> conns,
  I4 first, I4 last, I4 nb_bnd_elts) const
{
  I8 conn_dim = {conns.extent(0)*conns.extent(1)};
  auto conn_1d = std_e::make_span(conns.data(),conn_dim);
  return newElements(name,type,conn_1d,first,last,nb_bnd_elts);
}

tree factory::newNgonElements(const std::string& name, std_e::span<I4> conns, I4 first, I4 last, I4 nb_bnd_elts) const {
  I4 ngon_type = NGON_n;
  return newElements(name,ngon_type,conns,first,last,nb_bnd_elts);
}
tree factory::newNfaceElements(const std::string& name, std_e::span<I4> conns, I4 first, I4 last) const {
  I4 nface_type = NFACE_n;
  return newElements(name,nface_type,conns,first,last,0);
}


tree factory::newPointList(const std::string& name, std_e::span<I4> pl) const {
  node_value pl_value = view_as_node_value(pl);
  pl_value.dims = {1,pl_value.dims[0]}; // required by SIDS (9.3: BC_t)
  return {name, "IndexArray_t", pl_value, {}};
}
tree factory::newBC(const std::string& name, const std::string& loc, std_e::span<I4> point_list) const {
  node_value bcType = create_string_node_value("FamilySpecified",alloc());
  tree location = newGridLocation(loc);
  tree point_list_node = newPointList("PointList",point_list);
  return {name, "BC_t", bcType, {location,point_list_node}};
}


tree factory::newFlowSolution(const std::string& name, const std::string& gridLoc) const {
  tree loc = newGridLocation(gridLoc);
  return {name, "FlowSolution_t", MT, {loc}};
}
tree factory::newRind(const std::vector<I4>& rind_planes) const {
  node_value value = create_node_value_1d(rind_planes,alloc());
  return {"Rind", "Rind_t", value, {}};
}
tree factory::newDiscreteData(const std::string& name, const std::string& gridLoc) const {
  tree loc = newGridLocation(gridLoc);
  return {name, "DiscreteData_t", MT, {loc}};
}
tree factory::newZoneSubRegion(const std::string& name, int dim, const std::string& gridLoc) const {
  node_value dim_value = create_node_value_1d({dim},alloc());
  tree loc = newGridLocation(gridLoc);
  return {name, "ZoneSubRegion_t", dim_value, {loc}};
}
tree factory::newBCDataSet(const std::string& name, const std::string& val, const std::string& gridLoc) const {
  tree loc = newGridLocation(gridLoc);
  node_value value = create_string_node_value(val,alloc());
  return {name, "BCDataSet_t", value, {loc}};
}
tree factory::newBCData(const std::string& name) const {
  return {name, "BCData_t", MT, {}};
}



tree factory::newGridLocation(const std::string& loc) const {
  node_value location = create_string_node_value(loc,alloc());
  return {"GridLocation","GridLocation_t",location,{}};
}
tree factory::newFamily(const std::string& name) const {
  return {name, "Family_t", MT, {}};
}
tree factory::newFamilyBC(const std::string& famName) const {
  node_value famNameValue = create_string_node_value(famName,alloc());
  return {"FamilyBC", "FamilyBC_t", famNameValue, {}};
}

tree factory::newDescriptor(const std::string& name, const std::string& val) const {
  node_value value = create_string_node_value(val,alloc());
  return {name, "Descriptor_t", value, {}};
}

tree factory::newGridConnectivityType(const std::string& gc_type) const {
  node_value gc_type_value = create_string_node_value(gc_type,alloc());
  return {"GridConnectivityType","GridConnectivityType_t",gc_type_value,{}};
}
tree factory::newGridConnectivity(const std::string& name, const std::string& z_donor_name, const std::string& loc, const std::string& connec_type) const {
  node_value z_donor_value = create_string_node_value(z_donor_name,alloc());
  tree location = newGridLocation(loc);
  tree grid_conn_type = newGridConnectivityType(connec_type);
  return {name, "GridConnectivity_t", z_donor_value, {location,grid_conn_type}};
}

/// node creation }

/// node removal {
bool factory::deallocate_node_value(node_value& val) const {
  return alloc().deallocate(val.data);
}
bool factory::deallocate_node(tree& t) const {
  return deallocate_node_value(t.value);
}
void factory::deallocate_tree(tree& t) const {
  deallocate_node(t);
  for (auto& c : t.children) {
    deallocate_tree(c);
  }
}

void factory::rm_child(tree& t, const tree& c) const {
  auto predicate = [&](const tree& child){ return &child==&c; };
  rm_child_by_predicate(t,predicate);
}
void factory::rm_child_by_name(tree& t, const std::string& name) const {
  auto predicate = [&](const tree& child){ return is_of_name(child,name); };
  rm_child_by_predicate(t,predicate);
}
void factory::rm_child_by_label(tree& t, const std::string& label) const {
  auto predicate = [&](const tree& child){ return is_of_label(child,label); };
  rm_child_by_predicate(t,predicate);
}
void factory::rm_children_by_label(tree& t, const std::string& label) const {
  auto predicate = [&](const tree& child){ return is_of_label(child,label); };
  rm_children_by_predicate(t,predicate);
}
/// node removal }

// factory }


} // cgns
