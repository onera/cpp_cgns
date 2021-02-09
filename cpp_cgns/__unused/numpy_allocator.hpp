#pragma once


#include <vector>
#include <algorithm>
#include "std_e/future/contract.hpp"


namespace cgns {


class allocator {
  public:
    allocator() = default;
    allocator(const allocator&) = delete;
    allocator& operator=(const allocator&) = delete;
    allocator(allocator&&) = default;
    allocator& operator=(allocator&&) = default;
    virtual ~allocator() noexcept = default;

    template<class T> friend
    T* allocate(allocator& a, size_t n) {
      return a.allocate(n*sizeof(T));
    }
    friend
    void deallocate(allocator& a, void* p) {
      return a.deallocate(p);
    }

  protected:
    virtual void* allocate(size_t n) = 0;
    virtual void deallocate(void* p) noexcept = 0;
};

class numpy_allocator final : allocator {
  public:
    void* allocate(size_t n) override {
      PyObject* py_array = PyArray_SimpleNew(1, n, NPY_BYTE);
      py_arrays.push_back(py_array);
      return PyArray_DATA(py_array);
    }

    auto position(void* p) {
      return std::find_if(
        begin(py_arrays),end(py_arrays),
        [p](PyObject* py_array){ return PyArray_DATA(py_array)==p; }
      );
    }
    bool owns_memory(void* p) {
      return position(p)!=end(py_arrays);
    }

    // conditionnal deallocation: only deallocate if has allocated the memory
    void deallocate(void* p) noexcept {
      auto pos = position(p);
      if (pos!=end(py_arrays)) {
        Py_DECREF(py_array);
        py_arrays.erase(pos);
      }
    }

    PyObject* retrieve_owning_array(void* p) noexcept {
      auto pos = position(p);
      STD_E_ASSERT(pos!=end(py_arrays));
      PyObject* py_array = *pos;
      py_arrays.erase(pos);
      return py_array; // memory now owned by caller
    }

    ~numpy_allocator() override noexcept {
      for (PyObject* py_array : py_arrays) {
        Py_DECREF(py_array);
      }
    }

  private:
    std::vector<PyObject*> py_arrays;
};

class cgns_allocator final {
  public:
    cgns_allocator() = default;
    cgns_allocator(const cgns_allocator&) = delete;
    cgns_allocator& operator=(const cgns_allocator&) = delete;
    cgns_allocator(cgns_allocator&&) = default;
    cgns_allocator& operator=(cgns_allocator&&) = default;

    template<class T>
    T* allocate(size_t n) {
      size_t sz = n*sizeof(T);
      void* p = malloc(sz);
      owned_ptrs.push_back(p);
      return (T*)p;
    }

    auto position(void* p) {
      return std::find(begin(owned_ptrs),end(owned_ptrs),p);
    }
    bool owns_memory(void* p) {
      return position(p)!=end(owned_ptrs);
    }

    // conditionnal deallocation: only deallocate if has allocated the memory
    void deallocate(void* p) noexcept {
      auto pos = position(p);
      if (pos!=end(owned_ptrs)) {
        free(p);
        owned_ptrs.erase(pos);
      }
    }

    ~cgns_allocator() noexcept {
      for (auto& p : owned_ptrs) {
        free(p);
      }
    }
  private:
    std::vector<void*> owned_ptrs;
};

inline
bool operator==(const cgns_allocator& x, const cgns_allocator& y) {
  return &x==&y; // since they are not copyable, only way to be equal is by equal address
}
inline
bool operator!=(const cgns_allocator& x, const cgns_allocator& y) {
  return !(x==y);
}


template<class T>
T* allocate(cgns_allocator& a, size_t n) {
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
    T* allocate(size_t n) {
      STD_E_ASSERT(alloc!=nullptr);
      return cgns::allocate<T>(*alloc,n);
    }

    void deallocate(T* p, size_t) noexcept { 
      // do nothing: deallocation should be done by the cgns_allocator
    }

  // comparisons
    template <class T0, class T1> friend
    bool operator==(const cgns_std_allocator<T0>& x, const cgns_std_allocator<T1>& y);
  private:
    cgns_allocator* alloc;
};

template <class T0, class T1> inline
bool operator==(const cgns_std_allocator<T0>& x, const cgns_std_allocator<T1>& y) {
  return *x.alloc==*y.alloc;
}
template <class T0, class T1> inline
bool operator!=(const cgns_std_allocator<T0>& x, const cgns_std_allocator<T1>& y) {
  return !(x==y);
}


} // cgns
