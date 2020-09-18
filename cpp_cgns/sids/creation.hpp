#pragma once


#include "cpp_cgns/node_manip.hpp"
#include "cpp_cgns/allocator.hpp"


namespace cgns {


// TODO I4 -> I4 or I8
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
    tree newCGNSBase(const std::string& name, int cellDim, int physDim) const;
    tree newUnstructuredZone(const std::string& name, const std::array<I4,3>& dims = {0,0,0}) const;
    tree newGridCoordinates(const std::string& name="GridCoordinates") const;
    tree newZoneBC() const;
    tree newZoneGridConnectivity() const;
    tree newPointRange(I4 first, I4 last) const;
    tree newElementRange(I4 first, I4 last) const;

    tree newElements(const std::string& name, I4 type, std_e::span<I4> connectivity, I4 first, I4 last, I4 nb_bnd_elts = 0) const;
    tree newHomogenousElements(const std::string& name, I4 type, md_array_view<I4,2> connectivity, I4 first, I4 last, I4 nb_bnd_elts=0) const;
    tree newNgonElements(const std::string& name, std_e::span<I4> connectivity, I4 first, I4 last, I4 nb_bnd_elts=0) const;
    tree newNfaceElements(const std::string& name, std_e::span<I4> connectivity, I4 first, I4 last) const;

    tree newPointList(const std::string& name, std_e::span<I4> pl) const;
    tree newBC(const std::string& name, const std::string& loc, std_e::span<I4> point_list) const;

    tree newGridLocation(const std::string& loc) const;
    tree newRind(const std::vector<I4>& rind_planes) const;

    tree newFamily(const std::string& name) const;
    tree newFamilyBC(const std::string& famName) const;
    tree newFlowSolution(const std::string& name, const std::string& gridLoc) const;
    tree newDiscreteData(const std::string& name, const std::string& gridLoc) const;
    tree newZoneSubRegion(const std::string& name, int dim, const std::string& gridLoc) const;
    tree newBCDataSet(const std::string& name, const std::string& val, const std::string& gridLoc) const;
    tree newBCData(const std::string& name) const;
    tree newDescriptor(const std::string& name, const std::string& val) const;

    tree newGridConnectivityType(const std::string& gc_type) const;
    tree newGridConnectivity(const std::string& name, const std::string& z_donor_name, const std::string& loc, const std::string& connec_type) const;

    tree newDataArray(const std::string& name, node_value value) const;

    tree newUserDefinedData(const std::string& name, node_value value = MT) const;
    tree newUserDefinedData(const std::string& name, const std::string& val) const;

    template<class I>
    tree newOrdinal(I i) const;
    // removal
    bool deallocate_node_value(node_value& val) const;
    void rm_child(tree& t, const tree& c) const;
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
    template<class Unary_predicate> void rm_children_by_predicate(tree& t, Unary_predicate p) const;
    template<class Unary_predicate> void rm_child_by_predicate(tree& t, Unary_predicate p) const;
  // data member
    cgns_allocator* const alloc_ptr;
};

template<class I>
tree factory::newOrdinal(I i) const {
  node_value val = create_node_value_1d({i},alloc());
  return {"Ordinal", val, {}, "Ordinal_t"};
}


} // cgns
