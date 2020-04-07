#include "cpp_cgns/node_manip.hpp"


namespace cgns {


bool is_of_name(const tree& tree, const std::string& name);
bool is_of_type(const tree& tree, const std::string& type);
bool is_one_of_types(const tree& tree, const std::vector<std::string>& types);

bool has_child_of_name(const tree& t, const std::string& name);
bool has_child_of_type(const tree& t, const std::string& name);

tree& get_child_by_name(tree& t, const std::string& name);
tree& get_child_by_type(tree& t, const std::string& type);
tree_range get_children_by_type(tree& t, const std::string& type);
tree_range get_children_by_types(tree& t, const std::vector<std::string>& types);
tree_range get_children_by_name_or_type(tree& t, const std::string& s);
// const versions {
const tree& get_child_by_name(const tree& t, const std::string& name);
const tree& get_child_by_type(const tree& t, const std::string& type);
const_tree_range get_children_by_type(const tree& t, const std::string& type);
const_tree_range get_children_by_types(const tree& t, const std::vector<std::string>& types);
const_tree_range get_children_by_name_or_type(const tree& t, const std::string& s);
// const versions }

tree& get_node_by_matching(tree& t, const std::string& gen_path);
tree_range get_nodes_by_matching(tree& t, const std::string& gen_path);



} // cgns
