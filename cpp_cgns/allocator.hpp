#pragma once


#include <vector>
#include <algorithm>
#include "std_e/future/contract.hpp"


namespace cgns {


// TODO create an abstract allocator class and make cpp_cgns function use it as the interface
class cgns_allocator final {
  public:
    cgns_allocator() = default;
    cgns_allocator(const cgns_allocator&) = delete;
    cgns_allocator& operator=(const cgns_allocator&) = delete;
    cgns_allocator(cgns_allocator&&) = default;
    cgns_allocator& operator=(cgns_allocator&&) = default;

    template<class T> auto
    allocate(size_t n) -> T* {
      size_t sz = n*sizeof(T);
      void* ptr = malloc(sz);
      owned_ptrs.push_back(ptr);
      return (T*)ptr;
    }

    auto
    position_in_owned_ptrs(void* ptr) {
      return std::find(begin(owned_ptrs),end(owned_ptrs),ptr);
    }
    auto
    owns_memory(void* ptr) -> bool {
      return position_in_owned_ptrs(ptr)!=end(owned_ptrs);
    }

    // conditionnal deallocation: only deallocate if has allocated the memory
    auto
    deallocate(void* ptr) -> bool {
      auto pos = position_in_owned_ptrs(ptr);
      if (pos!=end(owned_ptrs)) {
        free(ptr);
        owned_ptrs.erase(pos);
        return true;
      }
      return false;
    }

    auto
    release_if_owner(void* ptr) -> bool {
      auto pos = position_in_owned_ptrs(ptr);
      if (pos!=end(owned_ptrs)) {
        owned_ptrs.erase(pos);
        return true;
      } else {
        return false;
      }
    }
    
    auto
    memory_destructor_function() const -> void(*)(void*) {
      return free;
    }

    ~cgns_allocator() {
      for (auto& ptr : owned_ptrs) {
        free(ptr);
      }
    }
  private:
    std::vector<void*> owned_ptrs;
};

inline auto
operator==(const cgns_allocator& x, const cgns_allocator& y) -> bool {
  return &x==&y; // since they are not copyable, only way to be equal is by equal address
}
inline auto
operator!=(const cgns_allocator& x, const cgns_allocator& y) -> bool {
  return !(x==y);
}


template<class T> auto
allocate(cgns_allocator& a, size_t n) -> T* {
  return a.template allocate<T>(n);
}



/**
  Standard-compliant allocator wrapping a cgns_allocator.
  Can be used as a std::vector allocator.
  Allocation is delegated to the cgns_allocator.
  Deallocation does nothing: if the client wants to deallocate, he should ask the cgns_allocator to do it
*/
template<class T>
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
    cgns_std_allocator()
      : alloc(nullptr)
    {}

    explicit
    cgns_std_allocator(cgns_allocator* alloc)
      : alloc(alloc)
    {}

  // alloc/dealloc
    auto
    allocate(size_t n) -> T* {
      STD_E_ASSERT(alloc!=nullptr);
      return cgns::allocate<T>(*alloc,n);
    }

    auto
    deallocate(T*, size_t) noexcept -> void {
      // do nothing: deallocation should be done by the cgns_allocator
    }

  // comparisons
    template <class T0, class T1> friend auto
    operator==(const cgns_std_allocator<T0>& x, const cgns_std_allocator<T1>& y) -> bool;
  private:
    cgns_allocator* alloc;
};

template <class T0, class T1> auto
operator==(const cgns_std_allocator<T0>& x, const cgns_std_allocator<T1>& y) -> bool {
  return *x.alloc==*y.alloc;
}
template <class T0, class T1> auto
operator!=(const cgns_std_allocator<T0>& x, const cgns_std_allocator<T1>& y) -> bool {
  return !(x==y);
}


} // cgns
