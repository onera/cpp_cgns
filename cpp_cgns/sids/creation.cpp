#include "cpp_cgns/sids/creation.hpp"
#include "cpp_cgns/tree_manip.hpp"
#include <functional>
#include "std_e/multi_array/utils.hpp"
#include "cpp_cgns/exception.hpp"


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


tree factory::newZoneBC() const {
  return {"ZoneBC", "ZoneBC_t", MT, {}};
}
tree factory::newZoneGridConnectivity() const {
  return {"ZoneGridConnectivity", "ZoneGridConnectivity_t", MT, {}};
}

tree factory::newGridCoordinates(const std::string& name) const {
  return {name, "GridCoordinates_t", MT, {}};
}




tree factory::newFlowSolution(const std::string& name, const std::string& gridLoc) const {
  tree loc = newGridLocation(gridLoc);
  return {name, "FlowSolution_t", MT, {loc}};
}
tree factory::newDiscreteData(const std::string& name, const std::string& gridLoc) const {
  tree loc = newGridLocation(gridLoc);
  return {name, "DiscreteData_t", MT, {loc}};
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
