#pragma once


#include <string>
#include "cpp_cgns/cgnslib.h"


namespace cpp_cgns {


int number_of_nodes(CGNS_ENUMT(ElementType_t) elt);
std::string cgns_element_to_string(CGNS_ENUMT(ElementType_t) elt);


} // cpp_cgns
