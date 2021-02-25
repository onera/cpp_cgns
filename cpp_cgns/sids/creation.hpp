#pragma once


#include "cpp_cgns/node_manip.hpp"
#include "cpp_cgns/exception.hpp"
#include "cpp_cgns/sids/cgnslib.h"
#include "std_e/buffer/buffer_vector.hpp"
#include <algorithm>
#include <initializer_list>


namespace cgns {

// We could give an allocator to all node creation functions (functions named "new_[CGNS_label]")
// We could even templatize it to accept any Buffer_allocator type
// For now, this is only done for nodes that pass data through std_e::buffer_vector, the other use the default allocator (malloc/free)

// creation according to SIDS
auto new_CGNSTree() -> tree;
auto new_CGNSVersionNode(R4 version = 3.1) -> tree;

auto new_GridCoordinates(const std::string& name="GridCoordinates") -> tree;
auto new_ZoneBC() -> tree;
auto new_ZoneGridConnectivity() -> tree;

auto new_GridLocation(const std::string& loc) -> tree;

auto new_Family(const std::string& name) -> tree;
auto new_FamilyBC(const std::string& famName) -> tree;
auto new_FlowSolution(const std::string& name, const std::string& gridLoc) -> tree;
auto new_DiscreteData(const std::string& name, const std::string& gridLoc) -> tree;
auto new_BCDataSet(const std::string& name, const std::string& val, const std::string& gridLoc) -> tree;
auto new_BCData(const std::string& name) -> tree;
auto new_Descriptor(const std::string& name, const std::string& val) -> tree;

auto new_GridConnectivityType(const std::string& gc_type) -> tree;
auto new_GridConnectivity(const std::string& name, const std::string& z_donor_name, const std::string& loc, const std::string& connec_type) -> tree;

auto
new_DataArray(const std::string& name, node_value&& value) -> tree;
template<class I, int N> auto
new_DataArray(const std::string& name, const I(&arr)[N]) -> tree;
template<class T, class A> auto
new_DataArray(const std::string& name, std_e::buffer_vector<T,A>&& v) -> tree;
template<class T, int rank, class A> auto
new_DataArray(const std::string& name, md_array<T,rank,A>&& arr) -> tree;
template<class T, int rank> auto
new_DataArray(const std::string& name, md_array_view<T,rank>& arr) -> tree;

auto
new_UserDefinedData(const std::string& name, node_value value = MT()) -> tree;
auto
new_UserDefinedData(const std::string& name, const std::string& val) -> tree;
template<class T> auto
new_UserDefinedData(const std::string& name, const T& val) -> tree;
template<class T, class A> auto
new_UserDefinedData(const std::string& name, std_e::buffer_vector<T,A>&& v) -> tree;

template<class I> auto
new_CGNSBase(const std::string& name, I cellDim, I physDim) -> tree;
template<class I> auto
new_UnstructuredZone(const std::string& name, const I(&dims)[3] = {0,0,0}) -> tree;
template<class I> auto
new_ZoneSubRegion(const std::string& name, I dim, const std::string& gridLoc) -> tree;
template<class I> auto
new_PointRange(I first, I last) -> tree;
template<class I> auto
new_ElementRange(I first, I last) -> tree;

template<class I, class A> auto
new_Elements(const std::string& name, I type, std_e::buffer_vector<I,A>&& connectivity, I first, I last, I nb_bnd_elts = 0) -> tree;
template<class I, class A> auto
new_Elements(const std::string& name, ElementType_t type, std_e::buffer_vector<I,A>&& connectivity, I first, I last, I nb_bnd_elts = 0) -> tree;
template<class I, class A> auto
new_HomogenousElements(const std::string& name, I type, md_array<I,2,A>&& connectivity, I first, I last, I nb_bnd_elts=0) -> tree;
template<class I, class A> auto
new_NgonElements(const std::string& name, std_e::buffer_vector<I,A>&& connectivity, I first, I last, I nb_bnd_elts=0) -> tree;
template<class I, class A> auto
new_NfaceElements(const std::string& name, std_e::buffer_vector<I,A>&& connectivity, I first, I last) -> tree;

template<class I, class A> auto
new_PointList(const std::string& name, std_e::buffer_vector<I,A>&& pl) -> tree;
template<class I> auto
new_PointList(const std::string& name, std::initializer_list<I> pl) -> tree;

template<class I, class A> auto
new_BC(const std::string& name, const std::string& loc, std_e::buffer_vector<I,A>&& point_list) -> tree;
template<class I> auto
new_BC(const std::string& name, const std::string& loc, std::initializer_list<I> pl) -> tree;

template<class I, class A> auto
new_Rind(std_e::buffer_vector<I,A>&& rind_planes) -> tree;

template<class I> auto
new_Ordinal(I i) -> tree;

// ====================== impl ======================

template<class I> auto
new_CGNSBase(const std::string& name, I cellDim, I physDim) -> tree {
  return {name, "CGNSBase_t", create_node_value({cellDim,physDim})};
}

template<class I> auto
new_UnstructuredZone(const std::string& name, const I(&dims)[3]) -> tree {
  tree z_type = {"ZoneType", "ZoneType_t", create_string_node_value("Unstructured"), {}};
  return {name, "Zone_t", create_node_value({{dims[0],dims[1],dims[2]}}), {std::move(z_type)}};
}
template<class I> auto
new_ZoneSubRegion(const std::string& name, I dim, const std::string& gridLoc) -> tree {
  return {name, "ZoneSubRegion_t", create_scalar_node_value(dim), {new_GridLocation(gridLoc)}};
}


template<class I> auto
new_PointRange(I first, I last) -> tree {
  return {"PointRange", "IndexRange_t", create_node_value({{first,last}})};
}
template<class I> auto
new_ElementRange(I first, I last) -> tree {
  return {"ElementRange", "IndexRange_t", create_node_value({first,last}), {}};
}


template<class I, class A> auto
new_Elements(
  const std::string& name, I type, std_e::buffer_vector<I,A>&& conns,
  I first, I last, I nb_bnd_elts)
-> tree
{
  return
    { name, "Elements_t", create_node_value({type,nb_bnd_elts}),
       { new_ElementRange(first,last) ,
         new_DataArray("ElementConnectivity", make_node_value(std::move(conns))) } };
}
template<class I, class A> auto
new_Elements(
  const std::string& name, ElementType_t type, std_e::buffer_vector<I,A>&& conns,
  I first, I last, I nb_bnd_elts)
-> tree
{
  return new_Elements(name,(I)type,std::move(conns),first,last,nb_bnd_elts);
}

template<class I, class A> auto
new_HomogenousElements(
  const std::string& name, I type, md_array<I,2,A>&& conns,
  I first, I last, I nb_bnd_elts)
-> tree {
  return new_Elements(name,type,std::move(conns.underlying_range()),first,last,nb_bnd_elts);
}

template<class I, class A> auto
new_NgonElements(const std::string& name, std_e::buffer_vector<I,A>&& conns, I first, I last, I nb_bnd_elts) -> tree {
  I ngon_type = NGON_n;
  return new_Elements(name,ngon_type,std::move(conns),first,last,nb_bnd_elts);
}
template<class I, class A> auto
new_NfaceElements(const std::string& name, std_e::buffer_vector<I,A>&& conns, I first, I last) -> tree {
  I nface_type = NFACE_n;
  return new_Elements(name,nface_type,std::move(conns),first,last,0);
}


template<class I, class A> auto
new_PointList(const std::string& name, std_e::buffer_vector<I,A>&& point_list) -> tree {
  node_value pl_value = make_node_value(std::move(point_list));
  pl_value.dims = {1,pl_value.dims[0]}; // required by SIDS (9.3: BC_t)
  return {name, "IndexArray_t", std::move(pl_value)};
}
template<class I> auto
new_PointList(const std::string& name, std::initializer_list<I> pl) -> tree {
  return new_PointList(name,std_e::make_buffer_vector(pl));
}

template<class I, class A> auto
new_BC(const std::string& name, const std::string& loc, std_e::buffer_vector<I,A>&& point_list) -> tree {
  return
    { name, "BC_t", create_string_node_value("FamilySpecified"),
       { new_GridLocation(loc),
         new_PointList("PointList",std::move(point_list)) } };
}
template<class I> auto
new_BC(const std::string& name, const std::string& loc, std::initializer_list<I> pl) -> tree {
  return new_BC(name,loc,std_e::make_buffer_vector(pl));
}

template<class I, class A> auto
new_Rind(std_e::buffer_vector<I,A>&& rind_planes) -> tree {
  return {"Rind", "Rind_t", create_node_value(std::move(rind_planes))};
}

template<class I> auto
new_Ordinal(I i) -> tree {
  return {"Ordinal", "Ordinal_t", create_scalar_node_value(i)};
}

template<class T> auto
new_UserDefinedData(const std::string& name, const T& val) -> tree {
  return {name, "UserDefinedData_t", create_scalar_node_value(val)};
}
template<class T, class A> auto
new_UserDefinedData(const std::string& name, std_e::buffer_vector<T,A>&& v) -> tree {
  return new_UserDefinedData(name,make_node_value(std::move(v)));
}

template<class T, int N> auto
new_DataArray(const std::string& name, const T(&arr)[N]) -> tree {
  return {name, "DataArray_t", create_node_value(arr)};
}
template<class T, class A> auto
new_DataArray(const std::string& name, std_e::buffer_vector<T,A>&& v) -> tree {
  return new_DataArray(name,make_node_value(std::move(v)));
}
template<class T, int rank, class A> auto
new_DataArray(const std::string& name, md_array<T,rank,A>&& arr) -> tree {
  return new_DataArray(name,make_node_value(std::move(arr)));
}
template<class T, int rank> auto
new_DataArray(const std::string& name, md_array_view<T,rank>& arr) -> tree {
  return new_DataArray(name,view_as_node_value(arr));
}


} // cgns
