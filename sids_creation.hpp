#pragma once


#include "cpp_cgns/cpp_cgns_utils.hpp"
#include "cpp_cgns/cgns_allocator.hpp"


namespace cpp_cgns {


struct Internal {
  public:
  // ctors
    Internal(cgns_allocator* alloc)
      : alloc_ptr(alloc)
    {}
    Internal(const Internal&) = delete;
    Internal& operator=(const Internal&) = delete;

    // accessors
    cgns_allocator& alloc() {
      return *alloc_ptr;
    }

    // creation according to SIDS
    tree newCGNSTree();
    tree newCGNSBase(const std::string& name, I4 cellDim, I4 physDim);
    tree newUnstructuredZone(const std::string& name, const std::array<I4,3>& dims);
    tree newGridCoordinates(const std::string& name="GridCoordinates");
    tree newZoneBC();
    tree newPointList(const std::string& name, std_e::span<I4> range);
    tree newElementRange(I4 first, I4 last);
    tree newRegularElements(const std::string& name, I4 type, md_array_view<I4,2>& connectivity, I4 first, I4 last, I4 nbEltsOnBoundary=0);
    tree newBC(const std::string& name, std_e::span<I4> point_list);
    tree newGridLocation(const std::string& loc);

    tree newFamily(const std::string& name);
    tree newFamilyBC(const std::string& famName);
    tree newFlowSolution(const std::string& name, const std::string& gridLocation);
    tree newDiscreteData(const std::string& name, const std::string& gridLocation);
    tree newBCDataSet(const std::string& name, const std::string& val, const std::string& gridLoc);
    tree newBCData(const std::string& name);
    tree newDescriptor(const std::string& name, const std::string& val);

    tree newDataArray(const std::string& name, node_value value);
    tree newUserDefinedData(const std::string& name, node_value value);

    tree newUserDefinedData(const std::string& name, const std::string& val);

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


} // cpp_cgns
