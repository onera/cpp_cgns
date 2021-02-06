#pragma once


#include "cpp_cgns/buffer_base.hpp"


namespace std_e {


class buffer {
  public:
    virtual auto is_owner() const -> bool = 0;
    virtual auto release() -> bool = 0;
    virtual auto deallocator() const -> deallocator_function = 0;

    virtual auto data()       ->       void* = 0;
    virtual auto data() const -> const void* = 0;

    virtual ~buffer() {}
};


class non_owning_buffer : public buffer {
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
class owning_buffer : public buffer {
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

  // buffer interface
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


} // std_e
