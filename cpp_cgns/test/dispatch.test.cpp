#include "std_e/unit_test/doctest.hpp"
#include "cpp_cgns/dispatch.hpp"
#include "std_e/base/lift.hpp"


using namespace cgns;

// Test dispatch_I4_I8
// Understanding what is going on with dispatch_I4_I8 in not straitforward
// So this test needs to define a bunch of stuff first

// Trees that we will be querying
I4 I4_value = 42;
I8 I8_value = 43;
const tree my_I4_tree = {"Test","Test_t", {"I4",{1},&I4_value}, {} };
const tree my_I8_tree = {"Test","Test_t", {"I8",{1},&I8_value}, {} };

// Template function that implements the tree query with the right type information
// The type I will either be I4 or I8
template<class I> auto
my_templated_query(const tree& node) -> I {
  auto ptr = (I*)node.value.data; // We need to know the type of the node value to correctly cast it
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

TEST_CASE("dispatch_I4_I8") {
  CHECK( my_query(my_I4_tree) == 42 );
  CHECK( my_query(my_I8_tree) == 43 );

  // Of course, we could call always call my_templated_query<I> instead of my_query
  // But then we need to retrieve the template parameter I
}
