#include "cpp_cgns/cpp_cgns_utils.hpp"


namespace cpp_cgns {


bool is_of_name(const tree& tree, const std::string& name);
bool is_of_type(const tree& tree, const std::string& type);
bool is_one_of_types(const tree& tree, const std::vector<std::string>& types);

tree& get_child_by_name(tree& t, const std::string& name);
tree& get_child_by_type(tree& t, const std::string& type);
tree_range get_children_by_type(tree& t, const std::string& type);
tree_range get_children_by_types(tree& t, const std::vector<std::string>& types);
tree_range get_children_by_name_or_type(tree& t, const std::string& s);

tree& get_node_by_matching(tree& t, const std::string& gen_path);
tree_range get_nodes_by_matching(tree& t, const std::string& gen_path);

bool has_child_of_name(tree& t, const std::string& name);
bool has_child_of_type(tree& t, const std::string& name);


} // cpp_cgns
