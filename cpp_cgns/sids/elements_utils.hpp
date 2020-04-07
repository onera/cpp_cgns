#pragma once


#include <string>
#include <vector>
#include "cpp_cgns/cgnslib.h"


namespace cgns {


int number_of_nodes(CGNS_ENUMT(ElementType_t) elt);
std::string cgns_element_to_string(CGNS_ENUMT(ElementType_t) elt);


std::vector<CGNS_ENUMT(ElementType_t)> element_types_of_dimension(int dim);


} // cgns
