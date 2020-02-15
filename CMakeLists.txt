cmake_minimum_required(VERSION 3.12)

## Project ##
project(
  cpp_cgns VERSION 0.1.0
  DESCRIPTION	"C++/CGNS mapping"
  LANGUAGES	CXX
)


## Find files ##
set(SRC_DIR ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/)

file(GLOB_RECURSE hpp_files
    "${SRC_DIR}*.hpp"
)

file(GLOB_RECURSE cpp_and_test_files
    "${SRC_DIR}*.cpp"
)
set(cpp_files ${cpp_and_test_files})
list(FILTER cpp_files EXCLUDE REGEX ".*\.test\.cpp$")
set(test_files ${cpp_and_test_files})
list(FILTER test_files INCLUDE REGEX ".*\.test\.cpp$")


## Compiler flags ##
add_compile_options(-Wall -fmax-errors=2)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -fno-math-errno -march=native -DNDEBUG -fopenmp -fPIC") # TODO move
set(CMAKE_BUILD_TYPE "Release") # TODO move


## Targets ##
add_subdirectory(extern/std_e)
find_package(Python REQUIRED COMPONENTS Interpreter Development)

add_library(${PROJECT_NAME} SHARED ${cpp_files})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)
target_link_libraries(${PROJECT_NAME}
  PUBLIC 
    std_e::std_e
    Python::Python
)

#add_subdirectory(extern/doctest)
#add_executable(${PROJECT_NAME}_unit_tests ${test_files})
#target_link_libraries(${PROJECT_NAME}_unit_tests PUBLIC ${PROJECT_NAME})
#target_link_libraries(${PROJECT_NAME}_unit_tests PRIVATE doctest::doctest)


## Install ##
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Targets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)
install(EXPORT ${PROJECT_NAME}Targets
  FILE ${PROJECT_NAME}.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION lib/cmake/${PROJECT_NAME}
)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
