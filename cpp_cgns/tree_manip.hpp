#include "cpp_cgns/node_manip.hpp"


namespace cgns {


bool is_of_name(const tree& tree, const std::string& name);
bool is_of_label(const tree& tree, const std::string& label);
bool is_one_of_labels(const tree& tree, const std::vector<std::string>& labels);

bool has_child_of_name(const tree& t, const std::string& name);
bool has_child_of_label(const tree& t, const std::string& name);

tree& get_child_by_name(tree& t, const std::string& name);
tree& get_child_by_label(tree& t, const std::string& label);
tree_range get_children_by_label(tree& t, const std::string& label);
tree_range get_children_by_labels(tree& t, const std::vector<std::string>& labels);
tree_range get_children_by_name_or_label(tree& t, const std::string& s);
// const versions {
const tree& get_child_by_name(const tree& t, const std::string& name);
const tree& get_child_by_label(const tree& t, const std::string& label);
const_tree_range get_children_by_label(const tree& t, const std::string& label);
const_tree_range get_children_by_labels(const tree& t, const std::vector<std::string>& labels);
const_tree_range get_children_by_name_or_label(const tree& t, const std::string& s);
// const versions }

tree& get_node_by_matching(tree& t, const std::string& gen_path);
tree_range get_nodes_by_matching(tree& t, const std::string& gen_path);



} // cgns
