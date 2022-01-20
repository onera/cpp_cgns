#pragma once

#include "cpp_cgns/cgns.hpp"

#include "cpp_cgns/sids/cgnslib.h"
#include "cpp_cgns/sids/elements_utils.hpp"
#include "std_e/interval/interval.hpp"
#include "std_e/interval/interval_sequence.hpp"


namespace cgns {


template<class I> using interval = std_e::closed_interval<I>; // CGNS intervals are always closed


auto
correct_bc_data_arrays_according_to_SIDS(tree& bcdata_node) -> void;

auto
nb_of_elements(const tree& elements_node) -> I8;


auto
element_type(const tree& elements_node) -> ElementType_t;
auto
element_range(const tree& element_node) -> cgns::interval<I8>;

auto
point_range_to_interval(const tree& pr) -> cgns::interval<I8>;


template<class I, class Tree> auto
regular_elements_connectivities(Tree& elements_node) -> md_array_view<I,2> {
  STD_E_ASSERT(label(elements_node)=="Elements_t");
  ElementType_t elt_type = element_type(elements_node);
  I8 n_node_for_elt_type = number_of_vertices(elt_type);
  I8 n_elt = nb_of_elements(elements_node);
  Tree& element_connectivity_node = get_child_by_name(elements_node, "ElementConnectivity");

  node_value elt_conn_val = element_connectivity_node.value;
  return view_as_md_array<I,2>(elt_conn_val,{n_node_for_elt_type,n_elt});
}


auto
compare_by_range(const tree& elts_node0, const tree& elts_node1) -> bool;
auto
equal_by_elt_type(const tree& elts_node0, const tree& elts_node1) -> bool;
auto
compare_by_elt_type(const tree& elts_node0, const tree& elts_node1) -> bool;


template<class I> auto
is_boundary(const md_array_view<I,2>& parent_elts, I i) -> bool;

template<class Array> auto
// requires Array, size==2, value_type==Integer
face_is_boundary(const Array& parent_elts) -> bool {
  return (parent_elts[0]==0) || (parent_elts[1]==0); // 0 means no parent element, as per CGNS SIDS 7.3
}


template<class Tree_range> auto
elts_ranges_are_contiguous(const Tree_range& elt_sections) -> bool {
  for (const tree& e : elt_sections) {
    STD_E_ASSERT(label(e)=="Elements_t");
  }
  STD_E_ASSERT(std::is_sorted(begin(elt_sections),end(elt_sections),compare_by_range));
  int n_section = elt_sections.size();
  for (int i=0; i<n_section-1; ++i) {
    auto current_range = element_range(elt_sections[i]);
    auto next_range = element_range(elt_sections[i+1]);
    return contiguous(current_range,next_range);
  }
  return true;
}
template<class I, class Tree_range> auto
element_sections_interval_vector(const Tree_range& elt_sections) -> std_e::interval_vector<I> {
  STD_E_ASSERT(elts_ranges_are_contiguous(elt_sections));

  int n_section = elt_sections.size();
  std_e::interval_vector<I> res(n_section); // TODO change ctor with +1
  for (int i=0; i<n_section; ++i) {
    res[i] = element_range(elt_sections[i]).first();
  }
  res[n_section] = element_range(elt_sections[n_section-1]).last()+1; // +1 because CGNS ranges are closed
  return res;
}

template<class Tree_range> auto
elts_types_are_unique(const Tree_range& elt_sections) -> bool {
  STD_E_ASSERT(std::is_sorted(begin(elt_sections),end(elt_sections),compare_by_elt_type));
  auto last = std::adjacent_find(begin(elt_sections),end(elt_sections),equal_by_elt_type);
  return last==end(elt_sections);
}

template<class I, class Tree> auto
PointList(Tree& t) {
  return get_child_value_by_name<I>(t,"PointList");
}
template<class I, class Tree> auto
PointListDonor(Tree& t) {
  return get_child_value_by_name<I>(t,"PointListDonor");
}


} // cgns
