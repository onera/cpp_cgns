## Global config ##
cmake_minimum_required(VERSION 3.12)
include(scripts/cmake/target_add_dependency.cmake)

## Project ##
project(
  cpp_cgns VERSION 0.1.0
  DESCRIPTION	"C++/CGNS mapping"
  LANGUAGES	CXX
)


## Find files ##
set(SRC_DIR ${PROJECT_SOURCE_DIR}/${PROJECT_NAME})
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR})

file(GLOB_RECURSE cpp_and_test_files
  "${SRC_DIR}/*.cpp"
)
set(cpp_files ${cpp_and_test_files})
list(FILTER cpp_files EXCLUDE REGEX ".*\.test\.cpp$")
set(test_files ${cpp_and_test_files})
list(FILTER test_files INCLUDE REGEX ".*\.test\.cpp$")


## Compiler flags ##
add_compile_options(-Wall -fmax-errors=2)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -fno-math-errno -march=native -DNDEBUG -fopenmp -fPIC") # TODO move


## Targets ##
add_library(${PROJECT_NAME} SHARED ${cpp_files})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${INCLUDE_DIR}>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)

target_add_dependency(${PROJECT_NAME} std_e)
target_add_thirdparty_dependency(${PROJECT_NAME} Python REQUIRED COMPONENTS Interpreter Development)
target_link_libraries(${PROJECT_NAME}
  PUBLIC 
    std_e::std_e
    Python::Python
)

add_executable(${PROJECT_NAME}_unit_tests ${test_files})
target_add_dependency(${PROJECT_NAME}_unit_tests doctest)
target_link_libraries(${PROJECT_NAME}_unit_tests
  PUBLIC
    ${PROJECT_NAME}
  PRIVATE
    doctest::doctest
)


## Install ##
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Targets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)
install(EXPORT ${PROJECT_NAME}Targets
  FILE ${PROJECT_NAME}Targets.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION lib/cmake/${PROJECT_NAME}
)
install(DIRECTORY ${SRC_DIR} DESTINATION include FILES_MATCHING PATTERN "*.hpp")
# TODO deps
#configure_file(scripts/cmake/fsdm_lib_config.cmake.in	${FSDM_LIB_NAME}Config.cmake @ONLY)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
