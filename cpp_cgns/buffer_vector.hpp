#pragma once


#include <vector>
#include <algorithm>
#include "std_e/future/contract.hpp"
#include "cpp_cgns/buffer_allocator.hpp"
#include "cpp_cgns/buffer.hpp"


namespace std_e {


// custom std allocator {
/**
  Standard-compliant allocator
  Can be used as a std::vector allocator.
  Allocation/Deallocation is delegated to the Buffer_allocator.
*/
template<class T, class Buffer_allocator> // TODO del default
// requires Buffer_allocator::allocate(size_t)/deallocate()
class std_buffer_allocator {
  public:
  // type traits
    using value_type = T;
    template<class U> friend class std_buffer_allocator;
    using propagate_on_container_copy_assignment = std::true_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_swap = std::true_type;

  // ctors
    explicit
    std_buffer_allocator(Buffer_allocator&& allocator = {})
      : allocator(std::move(allocator))
      , owner(true)
    {}

  // alloc/dealloc
    auto
    allocate(size_t n) -> T* {
      return allocator.allocate(n*sizeof(T));
    }

    auto
    deallocate(T*, size_t) noexcept -> void {
      if (owner) {
        allocator.deallocate();
      }
    }

    auto
    release_ownership() && -> void {
      owner = false;
    }
  // comparisons
    template <class T0, class T1, class A> friend auto
    operator==(const std_buffer_allocator<T0,A>& x, const std_buffer_allocator<T1,A>& y) -> bool;
  private:
    Buffer_allocator allocator;
    bool owner;
};

template <class T0, class T1, class A> auto
operator==(const std_buffer_allocator<T0,A>& x, const std_buffer_allocator<T1,A>& y) -> bool {
  return x.allocator==y.allocator;
}
template <class T0, class T1, class A> auto
operator!=(const std_buffer_allocator<T0,A>& x, const std_buffer_allocator<T1,A>& y) -> bool {
  return !(x==y);
}
// custom std allocator }


template<class T, class A = buffer_mallocator> using buffer_vector = std::vector<T, std_buffer_allocator<T,A>>;


// creation {
template<class T, class A = buffer_mallocator> auto
make_buffer_vector(A alloc = {}) {
  return buffer_vector<T,A>(std_buffer_allocator<T,A>(std::move(alloc)));
}

template<class T, class A = buffer_mallocator> auto
make_buffer_vector(size_t n, A alloc = {}) {
  return buffer_vector<T,A>(n,std_buffer_allocator<T,A>(std::move(alloc)));
}

template<class T, class A = buffer_mallocator> auto
make_buffer_vector(std::initializer_list<T> l, A alloc = {}) {
  return buffer_vector<T,A>(l,std_buffer_allocator<T,A>(std::move(alloc)));
}
// creation }



template<class T, class A> auto
to_buffer(buffer_vector<T,A>&& x) {
  using buffer_type = owning_buffer<A::dealloc_function>;
  x.release_ownership();
  return buffer_type(x.data());
}


} // std_e
