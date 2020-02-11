cmake_minimum_required(VERSION	3.1...3.12)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

## Project ##
set(PROJECT_NAME "cpp_cgns")
project(
  ${PROJECT_NAME} VERSION 0.1.0
  DESCRIPTION	"C++/CGNS mapping"
  LANGUAGES	CXX
)

## Find files ##
set(CPP_CGNS_DIR ../)

set(SRC_DIR ${CPP_CGNS_DIR}${PROJECT_NAME}/)
file(GLOB_RECURSE cpp_and_test_files
    "${SRC_DIR}*.cpp"
)

set(cpp_files ${cpp_and_test_files})
list(FILTER cpp_files EXCLUDE REGEX ".*\.test\.cpp$")

set(test_files ${cpp_and_test_files})
list(FILTER test_files INCLUDE REGEX ".*\.test\.cpp$")


## Libraries ##
set(STD_E_DIR ${CPP_CGNS_DIR}/extern/std_e/)
set(DOCTEST_DIR ${CPP_CGNS_DIR}/extern/doctest/)
find_package(Python REQUIRED COMPONENTS Interpreter Development)


## Compiler flags ##
set(CMAKE_CXX_FLAGS  "-Wall -fmax-errors=2")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -fno-math-errno -march=native -DNDEBUG -fopenmp -fPIC")
set(CMAKE_BUILD_TYPE "Release")

## Targets ##
add_library(cpp_cgns SHARED ${cpp_files})
target_compile_features(cpp_cgns PUBLIC cxx_std_17)
target_include_directories(cpp_cgns PUBLIC ${CPP_CGNS_DIR})
target_include_directories(cpp_cgns PUBLIC ${STD_E_DIR})
target_include_directories(cpp_cgns SYSTEM PUBLIC ${Python_INCLUDE_DIRS})

add_executable(cpp_cgns_unit_tests ${test_files})
target_include_directories(cpp_cgns_unit_tests PUBLIC ${DOCTEST_DIR})
target_link_libraries(cpp_cgns_unit_tests PUBLIC cpp_cgns)
target_link_libraries(cpp_cgns_unit_tests PUBLIC ${Python_LIBRARIES})
