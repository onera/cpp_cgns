#pragma once


#include "cpp_cgns/base/tree.hpp"


namespace cgns {


template<class Tree>
class tree_range_iterator {
  private:
    using vector_it = typename std::vector<Tree*>::iterator;
    using I = typename vector_it::difference_type;
    vector_it v_it;
  public:
    /// std::iterator type traits
    using value_type = Tree;
    using reference = Tree&;
    using difference_type = I;
    using iterator_category = std::random_access_iterator_tag;

    tree_range_iterator() = default;
    tree_range_iterator(vector_it v_it)
      : v_it(v_it)
    {}

    auto operator++() -> tree_range_iterator& {
      ++v_it;
      return *this;
    }
    auto operator++(int) -> tree_range_iterator {
      tree_range_iterator tmp(*this);
      ++*this;
      return tmp;

    }

    auto operator+=(I i) -> tree_range_iterator& {
      v_it += i;
      return *this;
    }
    auto operator-=(I i) -> tree_range_iterator& {
      v_it -= i;
      return *this;
    }
    friend
    auto operator+(tree_range_iterator it, I i) -> tree_range_iterator {
      tree_range_iterator res = it;
      res += i;
      return res;
    }
    friend
    auto operator-(tree_range_iterator it, I i) -> tree_range_iterator {
      tree_range_iterator res = it;
      res -= i;
      return res;
    }

    auto operator*() const -> Tree& {
      return **v_it;
    }

    auto data() const -> I* {
      return v_it;
    }

    friend constexpr auto
    operator==(const tree_range_iterator& it0, const tree_range_iterator& it1) {
      return it0.v_it == it1.v_it;
    }
    friend constexpr auto
    operator!=(const tree_range_iterator& it0, const tree_range_iterator& it1) {
      return !(it0 == it1);
    }
};


} // cgns
template<class Tree>
struct std::iterator_traits<cgns::tree_range_iterator<Tree>> {
  using type = cgns::tree_range_iterator<Tree>;
  using value_type = typename type::value_type;
  using reference = typename type::reference;
  using difference_type = typename type::difference_type;
  using iterator_category = typename type::iterator_category;
};
namespace cgns {


template<class Tree>
  requires (std::is_same_v<Tree,tree> || std::is_same_v<Tree,const tree>)
class Tree_range {
  private:
    std::vector<Tree*> v;
  public:
  // type traits
    using iterator = tree_range_iterator<Tree>;
    using const_iterator = tree_range_iterator<const Tree>;

  // ctors
    Tree_range() = default;
    Tree_range(int n)
      : v(n)
    {}

  // array
    auto
    size() const -> int {
      return v.size();
    }
    //auto
    //operator[](int i) -> Tree& {
    //  return *v[i];
    //}
    auto
    operator[](int i) const -> Tree& {
      return *v[i];
    }

  // range
    auto
    begin() -> iterator {
      return {v.begin()};
    }
    auto
    begin() const -> const_iterator {
      return {v.begin()};
    }
    auto
    end() -> iterator {
      return {v.end()};
    }
    auto
    end() const -> const_iterator {
      return {v.end()};
    }

  // container
    auto
    emplace_back(Tree& t) -> Tree& {
      return *v.emplace_back(&t);
    }
};


using tree_range = Tree_range<tree>;
using const_tree_range  = Tree_range<const tree>;


} // cgns
