# ------------------------------------------------------------------------------
# General CMake settings
# ------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
cmake_policy(SET CMP0074 NEW) # force find_package to take <PackageName>_ROOT variables into account

if(NOT DEFINED PROJECT_ROOT)
  set(PROJECT_ROOT ${CMAKE_SOURCE_DIR} CACHE PATH "Root directory, where the submodules are populated")
endif()

# Bootstrap project_utils
set(sub_repo_path "${PROJECT_ROOT}/external/project_utils")
file(GLOB sub_repo_files ${sub_repo_path}/*)
list(LENGTH sub_repo_files sub_repo_nb_files)
if(sub_repo_nb_files EQUAL 0)
  message(FATAL_ERROR
    "${sub_repo_path} is empty.
    Maybe you forgot to initialize it with \"git submodule update --init\""
  )
endif()
set(PROJECT_UTILS_DIR ${PROJECT_ROOT}/external/project_utils)
set(PROJECT_UTILS_CMAKE_DIR ${PROJECT_UTILS_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_UTILS_CMAKE_DIR}/find_package") # for custom Find*.cmake files
include(${PROJECT_UTILS_CMAKE_DIR}/dependency_management.cmake) # project_add_subdirectory, project_find_package and target_install

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")


# ------------------------------------------------------------------------------
# Project
# ------------------------------------------------------------------------------
project(
  cpp_cgns VERSION 0.1.0
  DESCRIPTION	"C++/CGNS mapping"
  LANGUAGES	CXX
)

option(${PROJECT_NAME}_ENABLE_FETCH_PYBIND "Fetch Pybind11 sources on-the-fly if not found by find_package()" ON)
option(${PROJECT_NAME}_ENABLE_COVERAGE "Enable coverage for ${PROJECT_NAME}" OFF)
option(${PROJECT_NAME}_ENABLE_DOCUMENTATION "Build ${PROJECT_NAME} documentation" OFF)
option(${PROJECT_NAME}_ENABLE_TESTS "Make CTest run the tests" ON)

## Compiler flags
### C++ standard
set(SUPPORTED_CXX_STANDARDS 17 20)
if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20 CACHE STRING "C++ standard to compile against")
endif()
if(NOT CMAKE_CXX_STANDARD IN_LIST SUPPORTED_CXX_STANDARDS)
  message(FATAL_ERROR "Supported CXX standards are: ${SUPPORTED_CXX_STANDARDS}.")
endif()

set(CMAKE_CXX_EXTENSIONS        OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON )
### fPIC
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
### Compiler-dependent flags
include(${PROJECT_UTILS_CMAKE_DIR}/default_flags.cmake)
### Default build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE) # default to Release
endif()
### Additionnal build types
include(${PROJECT_UTILS_CMAKE_DIR}/additional_build_types.cmake)

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------
include(${PROJECT_UTILS_CMAKE_DIR}/find_or_fetch.cmake)
find_or_fetch_pybind11()
### std_e ###
project_add_subdir_or_package(std_e REQUIRED)
### Python ###
if (NOT TARGET Python::Python OR NOT TARGET Python::NumPy)
  project_find_package(Python REQUIRED COMPONENTS Development NumPy)
endif()


# ------------------------------------------------------------------------------
# Compilation
# ------------------------------------------------------------------------------
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

target_link_libraries(${PROJECT_NAME}
  PUBLIC
    std_e::std_e
    pybind11::pybind11_headers
    Python::Python
    Python::NumPy
)


# ------------------------------------------------------------------------------
# Install
# ------------------------------------------------------------------------------
target_install(${PROJECT_NAME})


# ------------------------------------------------------------------------------
# Testing
# ------------------------------------------------------------------------------
include(CTest)
if (${PROJECT_NAME}_ENABLE_TESTS)
  find_or_fetch_doctest()
  add_subdirectory(test)
endif()

# ------------------------------------------------------------------------------
# Build documentation
# ------------------------------------------------------------------------------
if(${PROJECT_NAME}_ENABLE_DOCUMENTATION)
  include(${PROJECT_UTILS_CMAKE_DIR}/build_documentation.cmake)
  build_sphinx_documentation()
endif()


# ------------------------------------------------------------------------------
# Coverage TODO clean
# ------------------------------------------------------------------------------
if(${PROJECT_NAME}_ENABLE_COVERAGE)
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


# ------------------------------------------------------------------------------
# Summary
# ------------------------------------------------------------------------------
get_directory_property(is_top_lvl_cmakelists PARENT_DIRECTORY)
if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
  include(FeatureSummary)
  feature_summary(WHAT ALL)
endif()
