#pragma once


#include "cpp_cgns/base/tree.hpp"


namespace cgns {


class ctree {
  private:
    std::string n;
    std::string l;
    node_value v;
    tree_children c;
  public:
    ctree(std::string n, std::string l)
      : n(std::move(n))
      , l(std::move(l))
    {}
    ctree(std::string n, std::string l, node_value&& v)
      : ctree(std::move(n),std::move(l))
      , v(std::move(v))
    {}
    ctree(std::string n, std::string l, node_value&& v, tree_children&& c)
      : ctree(std::move(n),std::move(l),std::move(v))
      , c(std::move(c))
    {}

    auto name ()          ->       std::string  & { return n; }
    auto name ()    const -> const std::string  & { return n; }
    auto label()          ->       std::string  & { return l; }
    auto label()    const -> const std::string  & { return l; }
    auto value()          ->       node_value   & { return v; }
    auto value()    const -> const node_value   & { return v; }
    auto children()       ->       tree_children& { return c; }
    auto children() const -> const tree_children& { return c; }
};


} // cgns
