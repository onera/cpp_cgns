#include "std_e/unit_test/doctest.hpp"
#include "cpp_cgns/tree_manip.hpp"
#include "std_e/log.hpp"


using namespace cgns;

// Graph interface {
auto first_root(      tree& t) ->       tree* { return  &t   ; }
auto first_root(const tree& t) -> const tree* { return  &t   ; }
auto  last_root(      tree& t) ->       tree* { return (&t)+1; }
auto  last_root(const tree& t) -> const tree* { return (&t)+1; }

auto first_child(      tree& t) ->       tree* { return t.children.data()                    ; }
auto first_child(const tree& t) -> const tree* { return t.children.data()                    ; }
auto  last_child(      tree& t) ->       tree* { return t.children.data() + t.children.size(); }
auto  last_child(const tree& t) -> const tree* { return t.children.data() + t.children.size(); }
// Graph interface }

#include "std_e/graph/algorithm/algo_nodes.hpp"

template<class Tree>
// requires Tree==tree or Tree==const tree
class visitor_for_matching_path {
  public:
    visitor_for_matching_path(const std::string& gen_path)
      : identifiers(std_e::split(gen_path,'/'))
      , depth(0)
    {}

    auto
    pre(Tree& t) -> bool {
      bool is_matching = identifiers[depth]==t.name || identifiers[depth]==t.label;
      int max_depth = identifiers.size();
      if (depth==max_depth-1 && is_matching) {
        ELOG(&t);
        matching_nodes.push_back(t);
      }
      return depth < max_depth  &&  is_matching;
    }

    auto
    post(Tree&) {}

    auto
    up(Tree&, Tree&) -> void {
      --depth;
    }
    auto
    down(Tree&, Tree&) -> void {
      ++depth;
    }

    auto
    retrieve_nodes() -> range_of_ref<Tree> {
      return std::move(matching_nodes);
    }
  private:
    std::vector<std::string> identifiers;
    int depth;
    range_of_ref<Tree> matching_nodes;
};

template<class Tree> auto
get_nodes_by_matching2(Tree& t, const std::string& gen_path) -> range_of_ref<Tree> {
  visitor_for_matching_path<Tree> v(name(t)+'/'+gen_path);
  graph::depth_first_prune_adjacencies(t,v);
  return v.retrieve_nodes();
}


TEST_CASE("get_nodes_by_matching") {
  tree t = {
    "A", "A_t", MT,
    {
      tree{"B", "B_t", MT, {}},
      tree{"C", "C_t", MT,
        {
          tree{"D", "A_t", MT, {}}
        }
      }
    }
  };

  SUBCASE("non-const version") {
    auto ns = get_nodes_by_matching2(t,"C/A_t");

    CHECK( ns.size() == 1 );

    const tree& n = ns[0];
    CHECK( n.name  == "D" );
    CHECK( n.label == "A_t" );
  }

  SUBCASE("const version") {
    const tree& ct = t;

    auto ns = get_nodes_by_matching2(ct,"C/A_t");

    CHECK( ns.size() == 1 );

    const tree& n = ns[0];
    CHECK( n.name  == "D" );
    CHECK( n.label == "A_t" );
  }
}
