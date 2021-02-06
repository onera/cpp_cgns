#pragma once


#include "std_e/future/contract.hpp"
#include <string>
#include <vector>
#include <memory>


namespace cgns {


using deallocator_function = void(*)(void*);

class data_buffer {
  public:
    virtual auto is_owner() const -> bool = 0;
    virtual auto release() -> bool = 0;
    virtual auto deallocator() const -> deallocator_function = 0;

    virtual auto data()       ->       void* = 0;
    virtual auto data() const -> const void* = 0;

    virtual ~data_buffer() {}
};


class non_owning_buffer : public data_buffer {
  private:
    void* ptr;
  public:
    non_owning_buffer(void* ptr)
      : ptr(ptr)
    {}

    auto is_owner() const -> bool override { return false; }
    auto release() -> bool override { return false; }
    auto deallocator() const -> deallocator_function override { return [](void*){}; }

    auto data()       ->       void* override { return ptr; }
    auto data() const -> const void* override { return ptr; }
};



template<deallocator_function dealloc>
class owning_buffer : public data_buffer {
  private:
    void* ptr;
    bool owns;
  public:
  // ctors / assign / dtor
    owning_buffer(void* ptr)
      : ptr(ptr)
      , owns(true)
    {}

    owning_buffer(const owning_buffer&) = delete;
    owning_buffer& operator=(const owning_buffer&) = delete;

    owning_buffer(owning_buffer&& old)
      : ptr(old.ptr)
      , owns(old.owns)
    {
      old.owns = false;
    }
    owning_buffer& operator=(owning_buffer&& old) {
      ptr = old.ptr;
      owns = old.owns;
      old.owns = false;
      return *this;
    }

    ~owning_buffer() {
      if (is_owner()) {
        dealloc(ptr);
      }
      owns = false;
    }

  // data_buffer interface
    auto is_owner() const -> bool override {
      return owns;
    }
    auto release() -> bool override {
      bool was_owner = owns;
      owns = false;
      return was_owner;
    }
    auto deallocator() const -> deallocator_function override { return dealloc; }

    auto data()       ->       void* override { return ptr; }
    auto data() const -> const void* override { return ptr; }
};


struct malloc_allocator {
  static auto
  allocate(size_t n) -> void* {
    return malloc(n);
  }
  static auto
  deallocate(void* ptr) -> void {
    free(ptr);
  }

  using buffer_type = owning_buffer<free>;
};

inline auto
operator==(const malloc_allocator& x, const malloc_allocator& y) -> bool {
  return true; // no state
  //return &x==&y; // since they are not copyable, only way to be equal is by equal address
}
inline auto
operator!=(const malloc_allocator& x, const malloc_allocator& y) -> bool {
  return !(x==y);
}


} // cgns
