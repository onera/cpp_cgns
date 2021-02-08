#pragma once


struct cgns_tree;


#if defined(__cplusplus)
extern "C" {
#endif

struct cgns_tree* cgns_create_tree();
void cgns_cleanup_tree(struct cgns_tree* t);

const char* cgns_name(struct cgns_tree* t);
void cgns_set_name(struct cgns_tree* t, const char* s);


const char* cgns_data_type(struct cgns_tree* t);
void cgns_set_data_type(struct cgns_tree* t, const char* s);

int cgns_nb_dims(struct cgns_tree* t);
int64_t cgns_dim(struct cgns_tree* t, int i);
void cgns_set_dims(struct cgns_tree* t, int nb_dims, int64_t* dims);

void* cgns_data(struct cgns_tree* t);
void cgns_set_data(struct cgns_tree* t, void* data);


int cgns_nb_children(struct cgns_tree* t);
struct cgns_tree* cgns_child(struct cgns_tree* t, int i);
void cgns_push_child(struct cgns_tree* t, struct cgns_tree* c);
void cgns_rm_children(struct cgns_tree* t);


const char* cgns_type(struct cgns_tree* t);
void cgns_set_type(struct cgns_tree* t, const char* s);

#if defined(__cplusplus)
} // extern "C"
#endif
