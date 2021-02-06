#pragma once


#include <cstddef>


namespace std_e {


using allocator_function =   void* (*) (size_t);
using deallocator_function = void  (*) (void* );


} // std_e
