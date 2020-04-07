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
    factory(const factory&) = delete;
    factory& operator=(const factory&) = delete;

    // accessors
    cgns_allocator& alloc() {
      return *alloc_ptr;
    }

    // creation according to SIDS
    tree newCGNSTree();
    tree newCGNSBase(const std::string& name, int cellDim, int physDim);
    tree newUnstructuredZone(const std::string& name, const std::array<I4,3>& dims);
    tree newGridCoordinates(const std::string& name="GridCoordinates");
    tree newZoneBC();
    tree newZoneGridConnectivity();
    tree newPointRange(I4 first, I4 last);
    tree newElementRange(I4 first, I4 last);

    tree newElements(const std::string& name, I4 type, std_e::span<I4> connectivity, I4 first, I4 last, I4 nb_bnd_elts = 0);
    tree newHomogenousElements(const std::string& name, I4 type, md_array_view<I4,2> connectivity, I4 first, I4 last, I4 nb_bnd_elts=0);
    tree newNgonElements(const std::string& name, std_e::span<I4> connectivity, I4 first, I4 last, I4 nb_bnd_elts=0);
    tree newNfaceElements(const std::string& name, std_e::span<I4> connectivity, I4 first, I4 last);

    tree newPointList(const std::string& name, std_e::span<I4> pl);
    tree newBC(const std::string& name, const std::string& loc, std_e::span<I4> point_list);

    tree newGridLocation(const std::string& loc);
    tree newRind(const std::vector<I4>& rind_planes);

    tree newFamily(const std::string& name);
    tree newFamilyBC(const std::string& famName);
    tree newFlowSolution(const std::string& name, const std::string& gridLoc);
    tree newDiscreteData(const std::string& name, const std::string& gridLoc);
    tree newZoneSubRegion(const std::string& name, int dim, const std::string& gridLoc);
    tree newBCDataSet(const std::string& name, const std::string& val, const std::string& gridLoc);
    tree newBCData(const std::string& name);
    tree newDescriptor(const std::string& name, const std::string& val);

    tree newGridConnectivityType(const std::string& gc_type);
    tree newGridConnectivity(const std::string& name, const std::string& z_donor_name, const std::string& loc, const std::string& connec_type);

    tree newDataArray(const std::string& name, node_value value);

    tree newUserDefinedData(const std::string& name, node_value value = MT);
    tree newUserDefinedData(const std::string& name, const std::string& val);

    template<class I>
    tree newOrdinal(I i);
    // removal
    void rm_child_by_name(tree& t, const std::string& name);
    void rm_child_by_type(tree& t, const std::string& type);
    void rm_children_by_type(tree& t, const std::string& type);
  private:
  // member functions
    // creation according to SIDS
    tree newRootNode();
    tree newCGNSVersionNode();
    // removal
    void deallocate_tree(tree& t);
    void deallocate_node(tree& t);
    template<class Unary_predicate> void rm_children_by_predicate(tree& t, Unary_predicate p);
    template<class Unary_predicate> void rm_child_by_predicate(tree& t, Unary_predicate p);
  // data member
    cgns_allocator* alloc_ptr;
};

template<class I>
tree factory::newOrdinal(I i) {
  node_value val = create_node_value_1d({i},alloc());
  return {"Ordinal", val, {}, "Ordinal_t"};
}

using Internal = factory; // TODO DEL


} // cgns
