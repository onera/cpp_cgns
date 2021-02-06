#pragma once


#include <string>
#include <vector>
#include <memory>

using I8 = std::int64_t;

class node_value_interface {
  public:
    virtual ~node_value_interface() {}

    virtual auto is_owner(void* ptr) const -> bool = 0;
    virtual auto deallocator_function() const -> void(*)(void*) = 0;

    virtual auto data_type()       ->       std::string&     = 0;
    virtual auto data_type() const -> const std::string&     = 0;
    virtual auto dims     ()       ->       std::vector<I8>& = 0;
    virtual auto dims     () const -> const std::vector<I8>& = 0;
    virtual auto data     ()       ->       void*            = 0;
    virtual auto data     () const -> const void*            = 0;
};

class non_owning_node_value : public node_value_interface {
  public:
    non_owning_node_value(std::string dt, std::vector<I8> dims, void* data)
      : dt(std::move(dt))
      , dims(std::move(dims))
      , data(std::move(data))
    {}

    virtual auto is_owner(void* ptr) const -> bool   { return false; }
    static auto no_op_deallocator(void*) -> void {}
    auto deallocator_function() const -> void(*)(void*) { return no_op_deallocator; }

    auto data_type()       ->       std::string&     { return dt  ; }
    auto data_type() const -> const std::string&     { return dt  ; }
    auto dims     ()       ->       std::vector<I8>& { return dims; }
    auto dims     () const -> const std::vector<I8>& { return dims; }
    auto data     ()       ->       void*            { return data; }
    auto data     () const -> const void*            { return data; }
  private:
    std::string dt;
    std::vector<I8> dims;
    void* data;
};

struct node_value_interface {
  virtual ~node_value_interface() {}

  virtual auto is_owner(void* ptr) const -> bool = 0;
  virtual auto release_if_owner(void* ptr) -> bool = 0;
  virtual auto deallocator_function() const -> void(*)(void*) = 0;

  virtual auto deallocate(void* ptr) -> bool = 0;

  virtual auto data_type()       ->       std::string&     = 0;
  virtual auto data_type() const -> const std::string&     = 0;
  virtual auto dims     ()       ->       std::vector<I8>& = 0;
  virtual auto dims     () const -> const std::vector<I8>& = 0;
  virtual auto data     ()       ->       void*            = 0;
  virtual auto data     () const -> const void*            = 0;
};



class node_value {
  public:
    auto is_owner(void* ptr) const -> bool;
    auto release_if_owner(void* ptr) -> bool;
    auto deallocator_function() const -> void(*)(void*);

    auto deallocate(void* ptr) -> bool;

    auto data_type()       ->       std::string&    ; // TODO enum
    auto data_type() const -> const std::string&    ; // TODO enum
    auto dims     ()       ->       std::vector<I8>&;
    auto dims     () const -> const std::vector<I8>&;
    auto data     ()       ->       void*           ;
    auto data     () const -> const void*           ;
  private:
    std::unique_ptr<node_value_interface>
};
