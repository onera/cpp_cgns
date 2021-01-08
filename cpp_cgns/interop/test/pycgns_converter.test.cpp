#include "std_e/unit_test/doctest.hpp"
#include "cpp_cgns/sids/creation.hpp"
#include "cpp_cgns/interop/pycgns_converter.hpp"
#include "pybind11/numpy.h"
#include "std_e/log.hpp" // TODO
#include <pybind11/embed.h> 


using namespace cgns;
namespace py = pybind11;


auto
conversion_cpp_test_case(factory F) -> tree {
  tree b = F.newCGNSBase("Base",2,2);
    tree z0 = F.newUnstructuredZone("Z0",{5,3,0});
      auto elt_con_z0 = make_cgns_vector({1,2,3, 2,3,4, 3,4,5},F.alloc());
      emplace_child( z0 , F.newElements("tris", TRI_3, std_e::make_span(elt_con_z0), 1, 3) );

    tree z1 = F.newUnstructuredZone("Z1",{7,4,0});
      auto elt_con_z1 = make_cgns_vector({5,6,7, 1,2,3, 2,3,4, 3,4,5},F.alloc());
      emplace_child( z1 , F.newElements("tris", TRI_3, std_e::make_span(elt_con_z1), 1, 4) );

    emplace_child(b,std::move(z0));
    emplace_child(b,std::move(z1));

  return b;
}


auto
conversion_py_test_case() -> py::list {
  std::vector<int> base_dims = {2,2};
  py::list b(4);
  b[0] = "Base";
  b[1] = py::array(py::dtype("i"),2,base_dims.data());
  b[2] = py::list(); b[3] = "CGNSBase_t";

  return b;
}



TEST_CASE("view_as_cpptree") {
  py::scoped_interpreter guard{};

  auto py_tree = conversion_py_test_case();
  view_as_cpptree(py_tree);
}
