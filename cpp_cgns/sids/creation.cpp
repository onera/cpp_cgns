#include "cpp_cgns/sids/creation.hpp"
#include "cpp_cgns/tree_manip.hpp"
#include <functional>
#include "std_e/multi_array/utils.hpp"
#include "cpp_cgns/exception.hpp"


namespace cgns {


auto
new_DataArray(const std::string& name, node_value&& value) -> tree {
  return {name, "DataArray_t", std::move(value)};
}
auto
new_UserDefinedData(const std::string& name, node_value value) -> tree {
  return {name, "UserDefinedData_t", std::move(value)};
}
auto
new_UserDefinedData(const std::string& name, const std::string& val) -> tree {
  return {name, "UserDefinedData_t", create_string_node_value(val)};
}

auto
new_CGNSVersionNode(R4 version) -> tree {
  return {"CGNSLibraryVersion", "CGNSLibraryVersion_t", create_scalar_node_value(version)};
}

auto
new_CGNSTree() -> tree {
  return { "CGNSTree", "CGNSTree_t", MT(), {new_CGNSVersionNode()} };
}


auto
new_ZoneBC() -> tree {
  return {"ZoneBC", "ZoneBC_t", MT()};
}
auto
new_ZoneGridConnectivity() -> tree {
  return {"ZoneGridConnectivity", "ZoneGridConnectivity_t", MT()};
}

auto
new_GridCoordinates(const std::string& name) -> tree {
  return {name, "GridCoordinates_t", MT()};
}




auto
new_FlowSolution(const std::string& name, const std::string& gridLoc) -> tree {
  return {name, "FlowSolution_t", MT(), {new_GridLocation(gridLoc)}};
}
auto
new_DiscreteData(const std::string& name, const std::string& gridLoc) -> tree {
  return {name, "DiscreteData_t", MT(), {new_GridLocation(gridLoc)}};
}
auto
new_BCDataSet(const std::string& name, const std::string& val, const std::string& gridLoc) -> tree {
  return {name, "BCDataSet_t", create_string_node_value(val), {new_GridLocation(gridLoc)}};
}
auto
new_BCData(const std::string& name) -> tree {
  return {name, "BCData_t", MT()};
}


auto
new_GridLocation(const std::string& loc) -> tree {
  return {"GridLocation","GridLocation_t",create_string_node_value(loc)};
}
auto
new_Family(const std::string& name) -> tree {
  return {name, "Family_t", MT()};
}
auto
new_FamilyBC(const std::string& famName) -> tree {
  return {"FamilyBC", "FamilyBC_t", create_string_node_value(famName)};
}

auto
new_Descriptor(const std::string& name, const std::string& val) -> tree {
  return {name, "Descriptor_t", create_string_node_value(val)};
}

auto
new_GridConnectivityType(const std::string& gc_type) -> tree {
  return {"GridConnectivityType","GridConnectivityType_t",create_string_node_value(gc_type)};
}
auto
new_GridConnectivity(const std::string& name, const std::string& z_donor_name, const std::string& loc, const std::string& connec_type) -> tree {
  return
    { name, "GridConnectivity_t", create_string_node_value(z_donor_name),
      { new_GridLocation(loc),
        new_GridConnectivityType(connec_type) } };
}


} // cgns
