#include "std_e/unit_test/doctest.hpp"

#include "cpp_cgns/base/node_value_conversion.hpp"
#include "cpp_cgns/dispatch.hpp"
#include "std_e/base/lift.hpp"


using namespace cgns;

// Test dispatch_I4_I8 -- complete use {
// Understanding what is going on with dispatch_I4_I8 in not straitforward
// So this test needs to define a bunch of stuff first

// Trees that we will be querying
C1 C1_value = 'a';
I4 I4_value = 42;
I8 I8_value = 10'000'000'000ll; // > max(I4)
R4 R4_value = 50.f;
R8 R8_value = 150.;

const node_value my_C1_node_value = node_value(C1_value);
const node_value my_I4_node_value = node_value(I4_value);
const node_value my_I8_node_value = node_value(I8_value);
const node_value my_R4_node_value = node_value(R4_value);
const node_value my_R8_node_value = node_value(R8_value);

const tree my_I4_tree = {"Test","Test_t", node_value(I4_value), {} };
const tree my_I8_tree = {"Test","Test_t", node_value(I8_value), {} };

// Template function that implements the tree query with the right type information
// The type I will either be I4 or I8
template<class I> auto
my_templated_query(const tree& node) -> I {
  auto ptr = data_as<I>(node.value); // We need to know the type of the node value to correctly cast it
  return *ptr; // if given my_I4_tree, will return 42
               // if given my_I8_tree, will return 43
}


// Auxiliary function to make it work (type tag overload trick)
template<class I> auto
my_query__impl(
  I, // the type tag. Only the type of the argument is used. Since the value is not used, it is not named
  const tree& node
)
 -> I8 // Must not depend on the template parameter. This is needed because the function "my_query" (see below) can only return one type
{
  return my_templated_query<I>(node);
}

// The final function
auto
my_query(const tree& node) -> I8 {
  return dispatch_I4_I8(
    LIFT(my_query__impl), // LIFT is needed to turn the templated function "my_query__impl" into the corresponding lambda
    node
  );
}

TEST_CASE("dispatch_I4_I8 -- complete use") {
  CHECK( my_query(my_I4_tree) == 42 );
  CHECK( my_query(my_I8_tree) == 10'000'000'000ll );

  // Of course, we could call always call my_templated_query<I> instead of my_query
  // But then we need to retrieve the template parameter I
}
// Test dispatch_I4_I8 -- complete use }




// Test dispatch_I4_I8 -- base {
/// see
auto dispatch_base_fun(I4) -> int { return 10; }
auto dispatch_base_fun(I8) -> int { return 20; }

auto
dispatch_base_top_level_fun(const std::string& type) -> I8 {
  return dispatch_I4_I8(
    type,
    LIFT(dispatch_base_fun)
  );
}
TEST_CASE("dispatch_I4_I8 -- base") {
  CHECK( dispatch_base_top_level_fun("I4") == 10 );
  CHECK( dispatch_base_top_level_fun("I8") == 20 );
}
// Test dispatch_I4_I8 -- base }




// Test dispatch_I4_I8 -- node_value version {
template<class I> auto
my_node_query__impl(I, const node_value& x) -> bool {
  auto ptr = data_as<I>(x);
  return *ptr > 100;
}
auto
my_node_query(const node_value& node) -> bool {
  return dispatch_I4_I8(
    LIFT(my_node_query__impl),
    node
  );
}

TEST_CASE("dispatch_I4_I8 -- node_value version") {
  // Same as previous, but dispatch a node_value instead of a node
  CHECK( my_node_query(my_I4_node_value) == false );
  CHECK( my_node_query(my_I8_node_value) == true  );
}
// Test dispatch_I4_I8 -- node_value version }




// Test dispatch_on_data_type {
auto
my_complete_node_query(const node_value& val) -> bool {
  return dispatch_on_data_type(
    LIFT(my_node_query__impl),
    val
  );
}
TEST_CASE("dispatch_on_data_type") {
  CHECK( my_complete_node_query(my_C1_node_value) == false );
  CHECK( my_complete_node_query(my_I4_node_value) == false );
  CHECK( my_complete_node_query(my_I8_node_value) == true  );
  CHECK( my_complete_node_query(my_R4_node_value) == false );
  CHECK( my_complete_node_query(my_R8_node_value) == true  );
}
// Test dispatch_on_data_type }
