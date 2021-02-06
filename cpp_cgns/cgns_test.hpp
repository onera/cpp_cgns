#pragma once


#include <string>
#include <vector>
#include <memory>


class memory_buffer {
  public:
    virtual auto is_owner() const -> bool = 0;
    virtual auto release() -> bool = 0;
    virtual auto deallocator_function() const -> void(*)(void*) = 0;

    virtual auto data()       ->       void* = 0;
    virtual auto data() const -> const void* = 0;

    virtual ~memory_buffer() {}
};

class non_owning_buffer : public memory_buffer {
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

class malloc_buffer : public memory_buffer {
  public:
    malloc_buffer(size_t n)
      : ptr(malloc(n))
      , owns(true)
    {}

    auto is_owner() const -> bool { return owns; }
    auto release() -> bool {
      bool was_owner = owns;
      owns = false;
      return was_owner;
    }
    auto deallocator_function() const -> void(*)(void*) { return free; }

    auto data()       ->       void* { return ptr; }
    auto data() const -> const void* { return ptr; }
  private:
    void* ptr;
    bool owns;
};


using I8 = std::int64_t;
