#include "cpp_cgns/cpp_cgns.hpp"
#include "cpp_cgns/c_interop.hpp"
#include <algorithm>


// Opaque type for C that contains the C++ cgns tree
struct cgns_tree {
  cpp_cgns::tree impl;
};


// creation {
cgns_tree* cgns_create_tree() {
  return new cgns_tree;
}
void cgns_cleanup_tree(cgns_tree* t) {
  delete t;
}
// creation }


// access functions {
/// name {
const char* cgns_name(cgns_tree* t) {
  return t->impl.name.c_str();
}
void cgns_set_name(cgns_tree* t, const char* s) {
  t->impl.name = s;
}
/// name }


/// node_value {
const char* cgns_data_type(cgns_tree* t) {
  return t->impl.value.data_type.c_str();
}
void cgns_set_data_type(cgns_tree* t, const char* s) {
  t->impl.value.data_type = s;
}

int cgns_nb_dims(cgns_tree* t) {
  return t->impl.value.dims.size();
}
int64_t cgns_dim(cgns_tree* t, int i) {
  return t->impl.value.dims[i];
}
void cgns_set_dims(cgns_tree* t, int nb_dims, int64_t* dims) {
  t->impl.value.dims = std::vector<cpp_cgns::I8>(nb_dims);
  std::copy_n(dims,nb_dims,begin(t->impl.value.dims));
}

void* cgns_data(cgns_tree* t) {
  return t->impl.value.data;
}
void cgns_set_data(cgns_tree* t, void* data) {
  t->impl.value.data = data;
}
/// node_value }


/// children {
int cgns_nb_children(cgns_tree* t) {
  return t->impl.children.size();
}
cgns_tree* cgns_child(cgns_tree* t, int i) {
  cpp_cgns::tree& child_i = t->impl.children[i];
  return reinterpret_cast<cgns_tree*>(&child_i); // cgns_tree is composed of a cpp_cgns::tree, so same memory layouts
}
void cgns_push_child(cgns_tree* t, cgns_tree* c) {
  t->impl.children.push_back(c->impl);
}
void cgns_rm_children(cgns_tree* t) {
  t->impl.children = {};
}
/// children }


/// type {
const char* cgns_type(cgns_tree* t) {
  return t->impl.type.c_str();
}
void cgns_set_type(cgns_tree* t, const char* s) {
  t->impl.type = s;
}
/// type }
// access functions }
