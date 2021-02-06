#pragma once


#include <vector>
#include <algorithm>
#include "std_e/future/contract.hpp"
#include "cpp_cgns/data_buffer.hpp"


namespace cgns {


/**
  Standard-compliant allocator
  Can be used as a std::vector allocator.
  Allocation/Deallocation is delegated to the Allocator.
*/
template<class T, class Allocator = malloc_buffer> // TODO del default
// requires Allocator::allocate(size_t)/deallocate()
class cgns_std_allocator {
  public:
  // type traits
    using value_type = T;
    template<class U> friend class cgns_std_allocator;
    using propagate_on_container_copy_assignment = std::true_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_swap = std::true_type;

  // ctors
    explicit
    cgns_std_allocator() = default;

    explicit
    cgns_std_allocator(Allocator&& allocator)
      : allocator(std::move(allocator))
    {}

  // alloc/dealloc
    auto
    allocate(size_t n) -> T* {
      return allocator.allocate(n*sizeof(T));
    }

    auto
    deallocate(T*, size_t) noexcept -> void {
      allocator.deallocate();
    }

    auto
    retrieve_buffer() && -> Allocator { // "&&" means: this function can only be called on a temporary
      return std::move(allocator);
    }
  // comparisons
    template <class T0, class T1, class A> friend auto
    operator==(const cgns_std_allocator<T0,A>& x, const cgns_std_allocator<T1,A>& y) -> bool;
  private:
    Allocator allocator;
};

template <class T0, class T1, class A> auto
operator==(const cgns_std_allocator<T0,A>& x, const cgns_std_allocator<T1,A>& y) -> bool {
  return x.allocator==y.allocator;
}
template <class T0, class T1, class A> auto
operator!=(const cgns_std_allocator<T0,A>& x, const cgns_std_allocator<T1,A>& y) -> bool {
  return !(x==y);
}


} // cgns
