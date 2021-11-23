//#include "std_e/unit_test/doctest.hpp"
//
//#include "cpp_cgns/cgns.hpp"
//#include "cpp_cgns/node_manip.hpp"
//#include "std_e/log.hpp"
//
//
//using namespace cgns;
//
//
//// for the tests below, fake_python_allocator represent an external allocation strategy (e.g. Python third-party library)
//constexpr auto fake_python_alloc = malloc; // imagine this is the Python allocation function
//constexpr auto fake_python_dealloc = free;
//using fake_python_allocator = std_e::buffer_allocator<fake_python_alloc,fake_python_dealloc>;
//
//constexpr auto my_test_alloc = malloc; // imagine that this is a specific allocator (GPU...)
//constexpr auto my_test_dealloc = free;
//using my_test_allocator = std_e::buffer_allocator<my_test_alloc,my_test_dealloc>;
//
//class fake_python_program {
//  public:
//    auto
//    create_tree() -> tree {
//      return {"RootNode", "UserDefinedData_t", create_node_value({10,11},fake_python_allocator{})};
//    }
//
//    auto
//    aquire_ptr(void* ptr, std_e::deallocator_function dealloc_fun) -> void {
//      acquired_ptrs.push_back(ptr);
//      acquired_ptr_deallocators.push_back(dealloc_fun);
//    }
//
//    ~fake_python_program() {
//      int sz = acquired_ptrs.size();
//      for (int i=0; i<sz; ++i) {
//        void* ptr = acquired_ptrs[i];
//        auto dealloc = acquired_ptr_deallocators[i];
//        dealloc(ptr);
//      }
//    }
//  private:
//    std::vector<void*> acquired_ptrs;
//    std::vector<std_e::deallocator_function> acquired_ptr_deallocators;
//};
//
//
//auto
//create_node(my_test_allocator& alloc) -> tree {
//  return {
//    "my_node",
//    "UserDefinedData_t",
//    create_node_value({42,43,44},alloc)
//  };
//}
//
//TEST_CASE("memory transfer") {
//  fake_python_program py;
//  tree t = py.create_tree();
//
//  my_test_allocator alloc;
//  emplace_child(
//    t,
//    create_node(alloc)
//  );
//
//  tree& sub_node = t.children[0];
//  CHECK( sub_node.name == "my_node" );
//
//  auto& buf = sub_node.value.buffer;
//  CHECK( buf.is_owner() );
//
//  // release memory
//  auto dealloc = buf.release();
//  // make Python own the memory now, and tell it which deallocator to use
//  py.aquire_ptr(buf.data(),dealloc);
//
//  // not owner but the memory is still here
//  CHECK_FALSE( buf.is_owner() );
//  I4* i_ptr = (I4*)buf.data();
//  CHECK( *i_ptr   == 42 );
//  CHECK( *i_ptr+1 == 43 );
//  CHECK( *i_ptr+2 == 44 );
//}
//
//
//TEST_CASE("use buffer_vector then tranfer to cgns node") {
//  std::vector<I4> some_numbers = {0,1,2,3,4,5,6,7,8};
//
//  auto even_numbers = std_e::make_buffer_vector<I4>(my_test_allocator{});
//  auto is_even = [](I4 i){ return i%2==0; };
//
//  // here we need a vector: we want to grow it efficiently because we can't precompute the size efficiently
//  std::copy_if(
//    begin(some_numbers),end(some_numbers),
//    std::back_inserter(even_numbers),
//    is_even
//  );
//
//  tree even_numbers_node = {
//    "EvenNumbers",
//    "DataArray_t",
//    make_node_value(std::move(even_numbers)) // the data and mem ownership of "even_numbers" has been transfered to the buffer
//  };
//
//  auto& buf = even_numbers_node.value.buffer;
//  CHECK( buf.is_owner() );
//  I4* ptr = (I4*)buf.data();
//  CHECK( ptr[0] == 0 );
//  CHECK( ptr[1] == 2 );
//  CHECK( ptr[2] == 4 );
//  CHECK( ptr[3] == 6 );
//  CHECK( ptr[4] == 8 );
//}
