## Global config ##
cmake_minimum_required(VERSION 3.12)

if(NOT DEFINED PROJECT_ROOT)
  set(PROJECT_ROOT ${CMAKE_SOURCE_DIR} CACHE PATH "Root directory, where the submodules are populated")
endif()

macro(check_sub_repo_exists sub_repo_name)
  set(sub_repo_path "${PROJECT_ROOT}/external/${sub_repo_name}")
  file(GLOB sub_repo_files ${sub_repo_path}/*)
  list(LENGTH sub_repo_files sub_repo_nb_files)
  if(sub_repo_nb_files EQUAL 0)
    message(FATAL_ERROR
      "${PROJECT_ROOT}/external/${sub_repo_name} is empty.
      Maybe you forgot to initialize it with \"git submodule update --init\""
    )
  endif()
endmacro()

check_sub_repo_exists(project_utils)
set(PROJECT_UTILS_DIR ${PROJECT_ROOT}/external/project_utils)
set(PROJECT_UTILS_CMAKE_DIR ${PROJECT_UTILS_DIR}/scripts/cmake)
include(${PROJECT_UTILS_CMAKE_DIR}/target_add_dependency.cmake) # target_add_dependency, target_add_thirdparty_dependency and target_install commands
list(APPEND CMAKE_MODULE_PATH "${PROJECT_UTILS_CMAKE_DIR}") # for custom Find*.cmake files

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

## Project ##
project(
  cpp_cgns VERSION 0.1.0
  DESCRIPTION	"C++/CGNS mapping"
  LANGUAGES	CXX
)


## Dependencies ##
### pybind11 ###
set(PYBIND11_INSTALL ON CACHE BOOL "cpp_cgns requires PyBind" FORCE)
include(FetchContent)
FetchContent_Declare(
  pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11.git
  GIT_TAG        v2.6
)
FetchContent_GetProperties(pybind11)
if(NOT pybind11_POPULATED)
  FetchContent_Populate(pybind11)
  add_subdirectory(${pybind11_SOURCE_DIR} ${pybind11_BINARY_DIR})
endif()


## Find files ##
set(src_dir ${PROJECT_SOURCE_DIR}/${PROJECT_NAME})
set(include_dir ${PROJECT_SOURCE_DIR})

file(GLOB_RECURSE cpp_and_test_files
  CONFIGURE_DEPENDS
  "${src_dir}/*.cpp"
)
set(cpp_files ${cpp_and_test_files})
list(FILTER cpp_files EXCLUDE REGEX ".*\\.test\\.cpp$")
list(FILTER cpp_files EXCLUDE REGEX ".*\\.pybind\\.cpp$")
set(test_files ${cpp_and_test_files})
list(FILTER test_files INCLUDE REGEX ".*\\.test\\.cpp$")


## Targets ##
add_library(${PROJECT_NAME} SHARED ${cpp_files})
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${include_dir}>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)

check_sub_repo_exists(std_e)
target_add_dependency(${PROJECT_NAME} std_e)

target_link_libraries(${PROJECT_NAME}
  PUBLIC
    std_e::std_e
    pybind11::pybind11_headers
)
if(CMAKE_VERSION VERSION_LESS 3.14)
  target_add_thirdparty_dependency(${PROJECT_NAME} Python REQUIRED COMPONENTS Interpreter Development)
  target_link_libraries(${PROJECT_NAME}
    PUBLIC
      Python::Python
  )
else()
  target_add_thirdparty_dependency(${PROJECT_NAME} Python REQUIRED COMPONENTS Interpreter Development NumPy)
  target_link_libraries(${PROJECT_NAME}
    PUBLIC
      Python::Python
      Python::NumPy
  )
endif()


## Install ##
target_install(${PROJECT_NAME})


## Unit tests ##
option(CPP_CGNS_ENABLE_TEST "Enable testing for ${PROJECT_NAME}" ON)
if(CPP_CGNS_ENABLE_TEST)
  enable_testing()
  include(CTest)
  add_executable(${PROJECT_NAME}_unit_tests
    ${test_files}
    ${PROJECT_UTILS_DIR}/src/unit_test_main.cpp
  )
  target_link_libraries(${PROJECT_NAME}_unit_tests
    PUBLIC
      ${PROJECT_NAME}
    PRIVATE
      doctest::doctest
      pybind11::embed
  )
  add_test(NAME ${PROJECT_NAME}_unit_tests COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_unit_tests)
endif()

option(CPP_CGNS_ENABLE_COVERAGE "Enable coverage for ${PROJECT_NAME}" OFF)
if(CPP_CGNS_ENABLE_COVERAGE)
  if(NOT CPP_CGNS_ENABLE_TEST)
    message(FATAL_ERROR "You need to enable testing to enable coverage: CPP_CGNS_ENABLE_TEST is OFF")
  endif()
  # TODO -> Make a correct exclude directory
  # NOTE : not working with gcc-9.2
  include(CodeCoverage)
  APPEND_COVERAGE_COMPILER_FLAGS()

  set(COVERAGE_EXCLUDES "/opt/tools/gcc-8.3/include/c++/8.3.0/ext"
                        "/opt/tools/gcc-8.3/include/c++/8.3.0/*"
                        "/opt/tools/gcc-8.3/include/c++/8.3.0/bits/*")
  SETUP_TARGET_FOR_COVERAGE_LCOV(
    NAME ${PROJECT_NAME}_coverage
    DEPENDENCIES ${PROJECT_NAME}_unit_tests
    EXECUTABLE ctest -j ${n_cores} # Executable in PROJECT_BINARY_DIR
  )
endif()
# --------------------------------------------------------------------------------

# ----------------------------------------------------------------------
option(CPP_CGNS_BUILD_DOCUMENTATION "Build ${PROJECT_NAME} documentation" OFF)
if(CPP_CGNS_BUILD_DOCUMENTATION)
  include(${PROJECT_UTILS_CMAKE_DIR}/build_documentation.cmake)
  build_documentation()
endif()
# ----------------------------------------------------------------------

# ----------------------------------------------------------------------
get_directory_property(top_lvl_cmakelists PARENT_DIRECTORY)
if(top_lvl_cmakelists STREQUAL ${PROJECT_SOURCE_DIR})
  include(FeatureSummary)
  FEATURE_SUMMARY(WHAT ALL)
endif()
# ----------------------------------------------------------------------
