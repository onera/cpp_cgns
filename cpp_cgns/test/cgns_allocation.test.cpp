#include "std_e/unit_test/doctest.hpp"
#include "cpp_cgns/cgns.hpp"
#include "cpp_cgns/allocator.hpp"
#include "cpp_cgns/array_utils.hpp"


using namespace cgns;


// for the tests below, fake_python_allocator represent an external allocation strategy (e.g. Python third-party library)
class fake_python_allocator final {
  public:
    fake_python_allocator() = default;
    fake_python_allocator(const fake_python_allocator&) = delete;
    fake_python_allocator& operator=(const fake_python_allocator&) = delete;
    fake_python_allocator(fake_python_allocator&&) = default;
    fake_python_allocator& operator=(fake_python_allocator&&) = default;

    template<class T>
    T* allocate(size_t n) {
      size_t sz = n*sizeof(T);
      void* ptr = malloc(sz);
      ptrs.push_back(ptr);
      return (T*)ptr;
    }

    ~fake_python_allocator() {
      for (auto& ptr : ptrs) {
        free(ptr);
      }
    }
  private:
    std::vector<void*> ptrs;
};

class fake_python_program {
  public:
    tree create_tree() {
      int sz = 2;
      I4* data = alloc.template allocate<I4>(sz);
      data[0] = 10;
      data[1] = 11;
      node_value value = {"I4",{sz},data};
      return {"RootNode", value, {}, "UserDefinedData_t"};
    }
  private:
    fake_python_allocator alloc;
};


tree create_node(cgns_allocator& alloc) {
  int sz = 3;
  I4* mem = allocate<I4>(alloc,sz);
  mem[0] = 42;
  mem[1] = 43;
  mem[2] = 44;

  return {
    "my_node",
    {"I4",{sz},mem},
    {}, // no children
    "UserDefinedData_t"
  };
}

TEST_CASE("cgns_allocation") {
  fake_python_program py;
  tree t = py.create_tree();

  cgns_allocator alloc;
  emplace_child(
    t,
    create_node(alloc)
  );

  SUBCASE("memory ownship") {
    CHECK( t.children[0].name == "my_node" );

    CHECK_FALSE( alloc.owns_memory(t.value.data)             );
    CHECK      ( alloc.owns_memory(t.children[0].value.data) );

    alloc.deallocate(t.value.data); // does nothing since does not own
    alloc.deallocate(t.children[0].value.data);

    CHECK_FALSE( alloc.owns_memory(t.value.data)             );
    CHECK_FALSE( alloc.owns_memory(t.children[0].value.data) );
  }

  SUBCASE("cgns_vector") {
    std::vector<I4> some_numbers = {0,1,2,3,4,5,6,7,8};
    
    { // open scope
      auto even_numbers = make_cgns_vector<I4>(alloc);
      auto is_even = [](I4 i){ return i%2==0; };
      
      // here we need a vector: we want to grow it efficiently because we don't know how to precompute the size
      std::copy_if(
        begin(some_numbers),end(some_numbers),
        std::back_inserter(even_numbers),
        is_even
      );
      
      I8 nb_even = even_numbers.size();
      tree even_numbers_node = {
        "EvenNumbers",
        {"I4",{nb_even},even_numbers.data()},
        {},
        "DataArray_t"
      };
      emplace_child(t,std::move(even_numbers_node));
    } // close scope: the vector "even_numbers" does not exist anymore...

    CHECK( t.children[1].name == "EvenNumbers" );
    CHECK( alloc.owns_memory(t.children[1].value.data) ); // ... however, the memory is owned by the cgns_allocator, hence still valid
    CHECK( ((I4*)t.children[1].value.data)[0] == 0 );
    CHECK( ((I4*)t.children[1].value.data)[1] == 2 );
    CHECK( ((I4*)t.children[1].value.data)[2] == 4 );
    CHECK( ((I4*)t.children[1].value.data)[3] == 6 );
    CHECK( ((I4*)t.children[1].value.data)[4] == 8 );
  }
}

