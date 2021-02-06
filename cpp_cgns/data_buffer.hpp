#pragma once


#include "std_e/future/contract.hpp"
#include <string>
#include <vector>
#include <memory>


namespace cgns {


class data_buffer {
  public:
    virtual auto is_owner() const -> bool = 0;
    virtual auto release() -> bool = 0;
    virtual auto deallocator_function() const -> void(*)(void*) = 0;

    virtual auto data()       ->       void* = 0;
    virtual auto data() const -> const void* = 0;

    virtual ~data_buffer() {}
};


class non_owning_buffer : public data_buffer {
  public:
    non_owning_buffer(void* ptr)
      : ptr(ptr)
    {}

    auto is_owner() const -> bool { return false; }
    auto release() -> bool { return false; }
    auto deallocator_function() const -> void(*)(void*) { return no_op_deallocator; }

    auto data()       ->       void* { return ptr; }
    auto data() const -> const void* { return ptr; }
  private:
    static auto no_op_deallocator(void*) -> void {}
    void* ptr;
};

class malloc_buffer : public data_buffer {
  private:
    void* ptr;
    bool owns;
  public:
  // alloc/dealloc
    auto allocate(size_t n) -> void* {
      deallocate();
      owns = true;
      ptr = malloc(n);
      return ptr;
    }
    auto deallocate() -> void {
      if (is_owner()) {
        free(ptr);
      }
      owns = false;
    }

  // ctors / assign / dtor
    malloc_buffer()
      : ptr(nullptr)
      , owns(false)
    {}
    malloc_buffer(size_t n)
      : ptr(malloc(n))
      , owns(true)
    {}

    malloc_buffer(const malloc_buffer&) = delete;
    malloc_buffer& operator=(const malloc_buffer&) = delete;

    malloc_buffer(malloc_buffer&& old)
      : ptr(old.ptr)
      , owns(old.owns)
    {
      old.owns = false;
    }
    malloc_buffer& operator=(malloc_buffer&& old) {
      ptr = old.ptr;
      owns = old.owns;
      old.owns = false;
      return *this;
    }

    ~malloc_buffer() {
      deallocate();
    }

  // data_buffer interface
    auto is_owner() const -> bool { return owns; }
    auto release() -> bool {
      bool was_owner = owns;
      owns = false;
      return was_owner;
    }
    auto deallocator_function() const -> void(*)(void*) { return free; }

    auto data()       ->       void* { return ptr; }
    auto data() const -> const void* { return ptr; }
};

inline auto
operator==(const malloc_buffer& x, const malloc_buffer& y) -> bool {
  return &x==&y; // since they are not copyable, only way to be equal is by equal address
}
inline auto
operator!=(const malloc_buffer& x, const malloc_buffer& y) -> bool {
  return !(x==y);
}


} // cgns
