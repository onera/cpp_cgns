#include "cpp_cgns/sids/creation.hpp"
#include "cpp_cgns/tree_manip.hpp"
#include <functional>
#include "std_e/multi_array/utils.hpp"
#include "cpp_cgns/exception.hpp"
#include "cpp_cgns/cgnslib.h"


namespace cpp_cgns {


// Internal {

/// node creation {
tree Internal::newDataArray(const std::string& name, node_value value) {
  return {name, value, {}, "DataArray_t"};
}
tree Internal::newUserDefinedData(const std::string& name, node_value value) {
  return {name, value, {}, "UserDefinedData_t"};
}
tree Internal::newUserDefinedData(const std::string& name, const std::string& val) {
  node_value value = create_string_node_value(val,alloc());
  return {name, value, {}, "UserDefinedData_t"};
}

tree Internal::newRootNode() {
  return {"CGNSTree", MT, {}, "CGNSTree_t"};
}

tree Internal::newCGNSVersionNode() {
  R4 version = 3.1;
  node_value version_arr = create_node_value_1d({version},alloc());
  return {"CGNSLibraryVersion", version_arr, {}, "CGNSLibraryVersion_t"};
}

tree Internal::newCGNSTree() {
  tree t = newRootNode();
  t.children.push_back(newCGNSVersionNode());
  return t;
}

tree Internal::newCGNSBase(const std::string& name, I4 cellDim, I4 physDim) {
  I4* ptr = allocate<I4>(alloc(),2);
  ptr[0] = cellDim;
  ptr[1] = physDim;
  node_value value = {to_string<I4>(),{2},ptr};

  return {name, value, {}, "CGNSBase_t"};
}

tree Internal::newUnstructuredZone(const std::string& name, const std::array<I4,3>& dims) {
  node_value z_type_value = create_string_node_value("Unstructured",alloc());
  tree z_type = {"ZoneType", z_type_value, {}, "ZoneType_t"};

  node_value dim_value = create_node_value_1d(dims,alloc());
  dim_value.dims = {1,dim_value.dims[0]}; // required by SIDS file mapping (Zone_t)
  return {name, dim_value, {z_type}, "Zone_t"};
}

tree Internal::newZoneBC() {
  return {std::string("ZoneBC"), MT, {}, "ZoneBC_t"};
}
tree Internal::newZoneGridConnectivity() {
  return {std::string("ZoneGridConnectivity"), MT, {}, "ZoneGridConnectivity_t"};
}

tree Internal::newGridCoordinates(const std::string& name) {
  return {name, MT, {}, "GridCoordinates_t"};
}


tree Internal::newPointRange(I4 first, I4 last) {
  node_value range_value = create_node_value_1d({first,last},alloc());
  range_value.dims = {1,2};
  return {"PointRange", range_value, {}, "IndexRange_t"};
}
tree Internal::newElementRange(I4 first, I4 last) {
  node_value range_value = create_node_value_1d({first,last},alloc());
  return {"ElementRange", range_value, {}, "IndexRange_t"};
}


tree Internal::newElements(
  const std::string& name, I4 type, std_e::span<I4> conns,
  I4 first, I4 last, I4 nb_bnd_elts)
{
  tree elts_conn_node = newDataArray("ElementConnectivity", view_as_node_value(conns));
  tree eltsRange = newElementRange(first,last);
  node_value elts_node_val = create_node_value_1d({type,nb_bnd_elts},alloc());
  return {name, elts_node_val, {eltsRange,elts_conn_node}, "Elements_t"};
}
tree Internal::newHomogenousElements(
  const std::string& name, I4 type, md_array_view<I4,2> conns,
  I4 first, I4 last, I4 nb_bnd_elts)
{
  I8 conn_dim = {conns.extent(0)*conns.extent(1)};
  auto conn_1d = std_e::make_span(conns.data(),conn_dim);
  return newElements(name,type,conn_1d,first,last,nb_bnd_elts);
}

tree Internal::newNgonElements(const std::string& name, std_e::span<I4> conns, I4 first, I4 last, I4 nb_bnd_elts) {
  I4 ngon_type = CGNS_ENUMV(NGON_n);
  return newElements(name,ngon_type,conns,first,last,nb_bnd_elts);
}
tree Internal::newNfaceElements(const std::string& name, std_e::span<I4> conns, I4 first, I4 last) {
  I4 nface_type = CGNS_ENUMV(NFACE_n);
  return newElements(name,nface_type,conns,first,last,0);
}


tree Internal::newPointList(const std::string& name, std_e::span<I4> pl) {
  node_value pl_value = view_as_node_value(pl);
  pl_value.dims = {1,pl_value.dims[0]}; // required by SIDS (9.3: BC_t)
  return {name, pl_value, {}, "IndexArray_t"};
}
tree Internal::newBC(const std::string& name, const std::string& loc, std_e::span<I4> point_list) {
  node_value bcType = create_string_node_value("FamilySpecified",alloc());
  tree location = newGridLocation(loc);
  tree point_list_node = newPointList("PointList",point_list);
  return {name, bcType, {point_list_node}, "BC_t"};
}


tree Internal::newFlowSolution(const std::string& name, const std::string& gridLoc) {
  tree loc = newGridLocation(gridLoc);
  return {name, MT, {loc}, "FlowSolution_t"};
}
tree Internal::newRind(const std::vector<I4>& rind_planes) {
  node_value value = create_node_value_1d(rind_planes,alloc());
  return {"Rind", value, {}, "Rind_t"};
}
tree Internal::newDiscreteData(const std::string& name, const std::string& gridLoc) {
  tree loc = newGridLocation(gridLoc);
  return {name, MT, {loc}, "DiscreteData_t"};
}
tree Internal::newZoneSubRegion(const std::string& name, int dim, const std::string& gridLoc) {
  node_value dim_value = create_node_value_1d({dim},alloc());
  tree loc = newGridLocation(gridLoc);
  return {name, dim_value, {loc}, "ZoneSubRegion_t"};
}
tree Internal::newBCDataSet(const std::string& name, const std::string& val, const std::string& gridLoc) {
  tree loc = newGridLocation(gridLoc);
  node_value value = create_string_node_value(val,alloc());
  return {name, value, {loc}, "BCDataSet_t"};
}
tree Internal::newBCData(const std::string& name) {
  return {name, MT, {}, "BCData_t"};
}



tree Internal::newGridLocation(const std::string& loc) {
  node_value location = create_string_node_value(loc,alloc());
  return {"GridLocation",location,{},"GridLocation_t"};
}
tree Internal::newFamily(const std::string& name) {
  return {name, MT, {}, "Family_t"};
}
tree Internal::newFamilyBC(const std::string& famName) {
  node_value famNameValue = create_string_node_value(famName,alloc());
  return {"FamilyBC", famNameValue, {}, "FamilyBC_t"};
}

tree Internal::newDescriptor(const std::string& name, const std::string& val) {
  node_value value = create_string_node_value(val,alloc());
  return {name, value, {}, "Descriptor_t"};
}

tree Internal::newGridConnectivityType(const std::string& gc_type) {
  node_value gc_type_value = create_string_node_value(gc_type,alloc());
  return {"GridConnectivityType",gc_type_value,{},"GridConnectivityType_t"};
}
tree Internal::newGridConnectivity(const std::string& name, const std::string& z_donor_name, const std::string& loc, const std::string& connec_type) {
  node_value z_donor_value = create_string_node_value(z_donor_name,alloc());
  tree location = newGridLocation(loc);
  tree grid_conn_type = newGridConnectivityType(connec_type);
  return {name, z_donor_value, {location,grid_conn_type}, "GridConnectivity_t"};
}

/// node creation }

/// node removal {
void Internal::deallocate_node(tree& t) {
  alloc().deallocate(t.value.data);
}
void Internal::deallocate_tree(tree& t) {
  deallocate_node(t);
  for (auto& c : t.children) {
    deallocate_tree(c);
  }
}
template<class Unary_predicate>
void Internal::rm_child_by_predicate(tree& t, Unary_predicate p) {
  auto& cs = t.children;
  auto pos = std::find_if(begin(cs),end(cs),p);
  if (pos==end(t.children)) {
    throw cgns_exception("No node to erase");
  } else {
    deallocate_tree(*pos);
    cs.erase(pos);
  }
}
template<class Unary_predicate>
void Internal::rm_children_by_predicate(tree& t, Unary_predicate p) {
  auto& cs = t.children;
  auto not_p = [p](const auto& x){ return !p(x); };
  auto pos = std::stable_partition(begin(cs),end(cs),not_p); // move nodes to be deleted at the end
  for_each(pos,end(cs),[this](tree& n){ deallocate_tree(n); });
  cs.erase(pos,end(cs));
}

void Internal::rm_child_by_name(tree& t, const std::string& name) {
  auto predicate = [&](const tree& child){ return is_of_name(child,name); };
  rm_child_by_predicate(t,predicate);
}
void Internal::rm_child_by_type(tree& t, const std::string& type) {
  auto predicate = [&](const tree& child){ return is_of_type(child,type); };
  rm_child_by_predicate(t,predicate);
}
void Internal::rm_children_by_type(tree& t, const std::string& type) {
  auto predicate = [&](const tree& child){ return is_of_type(child,type); };
  rm_children_by_predicate(t,predicate);
}
/// node removal }

// Internal }


} // cpp_cgns
