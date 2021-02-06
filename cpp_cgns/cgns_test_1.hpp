#pragma once


#include <string>
#include <vector>
#include <memory>


class data_manager_interface {
  public:
    virtual auto is_owner() const -> bool = 0;
    virtual auto release() -> bool = 0;
    virtual auto deallocator_function() const -> void(*)(void*) = 0;

    virtual auto get()       ->       void* = 0;
    virtual auto get() const -> const void* = 0;

    virtual ~data_manager_interface() {}
};

class non_owning_data_manager : public data_manager_interface {
  public:
    non_owning_data_manager(void* ptr)
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

class malloc_data_manager : public data_manager_interface {
  public:
    malloc_data_manager(size_t n)
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


class data_manager {
  public:
    data_manager() = default;
    data_manager(const data_manager&) = delete;
    data_manager& operator=(const data_manager&) = delete;
    data_manager(data_manager&&) = default;
    data_manager& operator=(data_manager&&) = default;

    template<
      class T,
      std::enable_if_t< !std::is_same_v<data_manager,std::decay_t<T>>, int> =0 // disable this ctor if T == data_manager
    >
    data_manager(T&& x)
      : impl( new T{std::forward<T>(x)} )
    {}

    auto is_owner() const -> bool;
    auto release() -> bool;
    auto deallocator_function() const -> void(*)(void*);

    auto get()       ->       void*;
    auto get() const -> const void*;
  private:
    std::unique_ptr<data_manager_interface> impl;
};

auto data_manager::
is_owner() const -> bool {
  return impl->is_owner();
}

auto data_manager::
release() -> bool {
  impl->release();
}

auto data_manager::
deallocator_function() const -> void(*)(void*) {
  return impl->deallocator_function();
}

auto data_manager::
get() -> void* {
  return impl->get();
}

auto data_manager::
get() const -> const void* {
  return impl->get();
}


using I8 = std::int64_t;
struct node_value {
  std::string data_type;
  std::vector<I8> dims;
  data_manager data;
};
