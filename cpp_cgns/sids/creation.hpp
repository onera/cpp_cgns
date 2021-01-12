#pragma once


#include "cpp_cgns/node_manip.hpp"
#include "cpp_cgns/allocator.hpp"
#include "cpp_cgns/exception.hpp"
#include "cpp_cgns/sids/cgnslib.h"


namespace cgns {


struct factory {
  public:
  // ctors
    factory(cgns_allocator* alloc)
      : alloc_ptr(alloc)
    {}

    // accessors
    cgns_allocator& alloc() const {
      return *alloc_ptr;
    }

    // creation according to SIDS
    tree newCGNSTree() const;
    tree newGridCoordinates(const std::string& name="GridCoordinates") const;
    tree newZoneBC() const;
    tree newZoneGridConnectivity() const;

    tree newGridLocation(const std::string& loc) const;

    tree newFamily(const std::string& name) const;
    tree newFamilyBC(const std::string& famName) const;
    tree newFlowSolution(const std::string& name, const std::string& gridLoc) const;
    tree newDiscreteData(const std::string& name, const std::string& gridLoc) const;
    tree newBCDataSet(const std::string& name, const std::string& val, const std::string& gridLoc) const;
    tree newBCData(const std::string& name) const;
    tree newDescriptor(const std::string& name, const std::string& val) const;

    tree newGridConnectivityType(const std::string& gc_type) const;
    tree newGridConnectivity(const std::string& name, const std::string& z_donor_name, const std::string& loc, const std::string& connec_type) const;

    tree newDataArray(const std::string& name, node_value value) const;

    tree newUserDefinedData(const std::string& name, node_value value = MT) const;
    tree newUserDefinedData(const std::string& name, const std::string& val) const;

    template<class I>
    tree newCGNSBase(const std::string& name, I cellDim, I physDim) const;
    template<class I>
    tree newUnstructuredZone(const std::string& name, const I(&dims)[3] = {0,0,0}) const;
    template<class I>
    tree newZoneSubRegion(const std::string& name, I dim, const std::string& gridLoc) const;
    template<class I>
    tree newPointRange(I first, I last) const;
    template<class I>
    tree newElementRange(I first, I last) const;

    template<class I>
    tree newElements(const std::string& name, I type, std_e::span<I> connectivity, I first, I last, I nb_bnd_elts = 0) const;
    template<class I>
    tree newElements(const std::string& name, ElementType_t type, std_e::span<I> connectivity, I first, I last, I nb_bnd_elts = 0) const;
    template<class I>
    tree newHomogenousElements(const std::string& name, I type, md_array_view<I,2> connectivity, I first, I last, I nb_bnd_elts=0) const;
    template<class I>
    tree newNgonElements(const std::string& name, std_e::span<I> connectivity, I first, I last, I nb_bnd_elts=0) const;
    template<class I>
    tree newNfaceElements(const std::string& name, std_e::span<I> connectivity, I first, I last) const;

    template<class I>
    tree newPointList(const std::string& name, std_e::span<I> pl) const;
    template<class I>
    tree newBC(const std::string& name, const std::string& loc, std_e::span<I> point_list) const;

    template<class I>
    tree newRind(const std::vector<I>& rind_planes) const;

    template<class I>
    tree newOrdinal(I i) const;

    template<class T>
    tree newUserDefinedData(const std::string& name, const T& val) const;
    template<class I, int N>
    tree newDataArray(const std::string& name, const I(&arr)[N]) const;

    // removal
    bool deallocate_node_value(node_value& val) const;
    void rm_child(tree& t, const tree& c) const;
    template<class Unary_predicate> void rm_children_by_predicate(tree& t, Unary_predicate p) const;
    template<class Unary_predicate> void rm_child_by_predicate(tree& t, Unary_predicate p) const;
    void rm_child_by_name(tree& t, const std::string& name) const;
    void rm_child_by_label(tree& t, const std::string& label) const;
    void rm_children_by_label(tree& t, const std::string& label) const;
  private:
  // member functions
    // creation according to SIDS
    tree newRootNode() const;
    tree newCGNSVersionNode() const;
    // removal
    bool deallocate_node(tree& t) const;
    void deallocate_tree(tree& t) const;
  // data member
    cgns_allocator* const alloc_ptr;
};




// ====================== impl ======================

/// node creation {
template<class I> auto factory::
newCGNSBase(const std::string& name, I cellDim, I physDim) const -> tree {
  I* ptr = allocate<I>(alloc(),2);
  ptr[0] = cellDim;
  ptr[1] = physDim;
  node_value value = {to_string<I>(),{2},ptr};

  return {name, "CGNSBase_t", value, {}};
}

template<class I> auto factory::
newUnstructuredZone(const std::string& name, const I(&dims)[3]) const -> tree {
  node_value z_type_value = create_string_node_value("Unstructured",alloc());
  tree z_type = {"ZoneType", "ZoneType_t", z_type_value, {}};

  const I* dims_ptr = dims;
  auto dims_span = std_e::make_span(dims_ptr,3);
  node_value dim_value = create_node_value_1d(dims_span,alloc());
  dim_value.dims = {1,dim_value.dims[0]}; // required by SIDS file mapping (Zone_t)
  return {name, "Zone_t", dim_value, {z_type}};
}
template<class I> auto factory::
newZoneSubRegion(const std::string& name, I dim, const std::string& gridLoc) const -> tree {
  node_value dim_value = create_node_value_1d({dim},alloc());
  tree loc = newGridLocation(gridLoc);
  return {name, "ZoneSubRegion_t", dim_value, {loc}};
}


template<class I> auto factory::
newPointRange(I first, I last) const -> tree {
  node_value range_value = create_node_value_1d({first,last},alloc());
  range_value.dims = {1,2};
  return {"PointRange", "IndexRange_t", range_value, {}};
}
template<class I> auto factory::
newElementRange(I first, I last) const -> tree {
  node_value range_value = create_node_value_1d({first,last},alloc());
  return {"ElementRange", "IndexRange_t", range_value, {}};
}


template<class I> auto factory::
newElements(
  const std::string& name, I type, std_e::span<I> conns,
  I first, I last, I nb_bnd_elts) const
-> tree
{
  tree elts_conn_node = newDataArray("ElementConnectivity", view_as_node_value(conns));
  tree eltsRange = newElementRange(first,last);
  node_value elts_node_val = create_node_value_1d({type,nb_bnd_elts},alloc());
  return {name, "Elements_t", elts_node_val, {eltsRange,elts_conn_node}};
}
template<class I> auto factory::
newElements(
  const std::string& name, ElementType_t type, std_e::span<I> conns,
  I first, I last, I nb_bnd_elts) const
-> tree 
{
  return newElements(name,(I)type,conns,first,last,nb_bnd_elts);
}

template<class I> auto factory::
newHomogenousElements(
  const std::string& name, I type, md_array_view<I,2> conns,
  I first, I last, I nb_bnd_elts) const
-> tree {
  I8 conn_dim = {conns.extent(0)*conns.extent(1)};
  auto conn_1d = std_e::make_span(conns.data(),conn_dim);
  return newElements(name,type,conn_1d,first,last,nb_bnd_elts);
}

template<class I> auto factory::
newNgonElements(const std::string& name, std_e::span<I> conns, I first, I last, I nb_bnd_elts) const -> tree {
  I ngon_type = NGON_n;
  return newElements(name,ngon_type,conns,first,last,nb_bnd_elts);
}
template<class I> auto factory::
newNfaceElements(const std::string& name, std_e::span<I> conns, I first, I last) const -> tree {
  I nface_type = NFACE_n;
  return newElements(name,nface_type,conns,first,last,0);
}


template<class I> auto factory::
newPointList(const std::string& name, std_e::span<I> pl) const -> tree {
  node_value pl_value = view_as_node_value(pl);
  pl_value.dims = {1,pl_value.dims[0]}; // required by SIDS (9.3: BC_t)
  return {name, "IndexArray_t", pl_value, {}};
}
template<class I> auto factory::
newBC(const std::string& name, const std::string& loc, std_e::span<I> point_list) const -> tree {
  node_value bcType = create_string_node_value("FamilySpecified",alloc());
  tree location = newGridLocation(loc);
  tree point_list_node = newPointList("PointList",point_list);
  return {name, "BC_t", bcType, {location,point_list_node}};
}

template<class I> auto factory::
newRind(const std::vector<I>& rind_planes) const -> tree {
  node_value value = create_node_value_1d(rind_planes,alloc());
  return {"Rind", "Rind_t", value, {}};
}

template<class I> auto factory::
newOrdinal(I i) const -> tree {
  node_value val = create_node_value_1d({i},alloc());
  return {"Ordinal", "Ordinal_t", val, {}};
}

template<class T> auto factory::
newUserDefinedData(const std::string& name, const T& val) const -> tree {
  node_value value = create_node_value_1d({val},alloc());
  return {name, "UserDefinedData_t", value, {}};
}

template<class T, int N> auto factory::
newDataArray(const std::string& name, const T(&arr)[N]) const -> tree {
  const T* arr_ptr = arr;
  auto arr_span = std_e::make_span(arr_ptr,N);
  node_value value = create_node_value_1d(arr_span,alloc());
  return {name, "DataArray_t", value, {}};
}
/// node creation }


/// node removal {
template<class Unary_predicate> auto factory::
rm_child_by_predicate(tree& t, Unary_predicate p) const -> void {
  auto& cs = t.children;
  auto pos = std::find_if(begin(cs),end(cs),p);
  if (pos==end(cs)) {
    throw cgns_exception("No node to erase");
  } else {
    deallocate_tree(*pos);
    cs.erase(pos);
  }
}
template<class Unary_predicate> auto factory::
rm_children_by_predicate(tree& t, Unary_predicate p) const -> void {
  auto& cs = t.children;
  auto not_p = [p](const auto& x){ return !p(x); };
  auto pos = std::stable_partition(begin(cs),end(cs),not_p); // move nodes to be deleted at the end
  for_each(pos,end(cs),[this](tree& n){ deallocate_tree(n); });
  cs.erase(pos,end(cs));
}
/// node removal }


} // cgns
