## Global config ##
cmake_minimum_required(VERSION 3.12)

if(DEFINED ENV{PROJECT_ROOT})
  set(PROJECT_ROOT $ENV{PROJECT_ROOT})
else()
  set(PROJECT_ROOT ${CMAKE_SOURCE_DIR})
endif()

list(APPEND CMAKE_MODULE_PATH ${PROJECT_ROOT}/external/project_utils/scripts/cmake)
include(target_add_dependency)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

## Project ##
project(
  cpp_cgns VERSION 0.1.0
  DESCRIPTION	"C++/CGNS mapping"
  LANGUAGES	C CXX
)


## Dependencies ##
include(FetchContent)
### doctest ###
FetchContent_Declare(
  doctest
  GIT_REPOSITORY https://github.com/onqtam/doctest.git
  GIT_TAG        2.3.7
)
FetchContent_GetProperties(doctest)
if(NOT doctest_POPULATED)
  FetchContent_Populate(doctest)
  add_subdirectory(${doctest_SOURCE_DIR} ${doctest_BINARY_DIR})
endif()


## Find files ##
set(src_dir ${PROJECT_SOURCE_DIR}/${PROJECT_NAME})
set(include_dir ${PROJECT_SOURCE_DIR})

file(GLOB_RECURSE cpp_and_test_files
  "${src_dir}/*.cpp"
)
set(cpp_files ${cpp_and_test_files})
list(FILTER cpp_files EXCLUDE REGEX ".*\.test\.cpp$")
set(test_files ${cpp_and_test_files})
list(FILTER test_files INCLUDE REGEX ".*\.test\.cpp$")


## Targets ##
add_library(${PROJECT_NAME} SHARED ${cpp_files})
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${include_dir}>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)

target_add_dependency(${PROJECT_NAME} std_e)

if(CMAKE_VERSION VERSION_LESS 3.14)
  target_add_thirdparty_dependency(${PROJECT_NAME} Python REQUIRED COMPONENTS Interpreter Development)
  target_link_libraries(${PROJECT_NAME}
    PUBLIC 
      std_e::std_e
      Python::Python
  )
else()
  target_add_thirdparty_dependency(${PROJECT_NAME} Python REQUIRED COMPONENTS Interpreter Development NumPy)
  target_link_libraries(${PROJECT_NAME}
    PUBLIC 
      std_e::std_e
      Python::Python
      Python::NumPy
  )
endif()


## Install ##
target_install(${PROJECT_NAME})

## CPython interface
add_library(py_${PROJECT_NAME} SHARED "${src_dir}/interop/py_${PROJECT_NAME}.cpp")
target_link_libraries(py_${PROJECT_NAME}
  PUBLIC 
    cpp_cgns::cpp_cgns
)
set_target_properties(
  py_${PROJECT_NAME}
  PROPERTIES
    PREFIX ""
    OUTPUT_NAME "${PROJECT_NAME}"
    LINKER_LANGUAGE C
)
install(TARGETS py_${PROJECT_NAME} EXPORT py_${PROJECT_NAME}Targets
  LIBRARY DESTINATION py
)


## Unit tests ##
option(CPP_CGNS_ENABLE_TEST "Enable testing for ${PROJECT_NAME}" ON)
if(CPP_CGNS_ENABLE_TEST)
  enable_testing()
  include(CTest)
  add_executable(${PROJECT_NAME}_unit_tests
    ${test_files}
    ${PROJECT_ROOT}/external/project_utils/src/unit_test_main.cpp
  )
  target_link_libraries(${PROJECT_NAME}_unit_tests
    PUBLIC
      ${PROJECT_NAME}
    PRIVATE
      doctest::doctest
  )
  add_test(NAME ${PROJECT_NAME}_unit_tests COMMAND ${PROJECT_NAME}_unit_tests)
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
  # Download bootstrap style
  FetchContent_Declare(
    sphinx_bootstrap_theme
    GIT_REPOSITORY https://github.com/ryan-roemer/sphinx-bootstrap-theme.git
    GIT_TAG        v0.8.0
  )
  FetchContent_GetProperties(sphinx_bootstrap_theme)
  if(NOT sphinx_bootstrap_theme_POPULATED)
    FetchContent_Populate(sphinx_bootstrap_theme)
  endif()

  include(UseBreathe)
  add_breathe_doc(TARGET_NAME ${PROJECT_NAME}_doc
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/doc
    DOXY_FILE  ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in
    CONF_FILE  ${CMAKE_CURRENT_SOURCE_DIR}/doc/conf.py.in
    BUILD_DIR  ${CMAKE_CURRENT_BINARY_DIR}/_build
    CACHE_DIR  ${CMAKE_CURRENT_BINARY_DIR}/_doctrees
    HTML_DIR   ${CMAKE_CURRENT_BINARY_DIR}/html
    COMMENT "HTML documentation"
  )

  install(DIRECTORY   ${CMAKE_CURRENT_BINARY_DIR}/html
          DESTINATION ${CMAKE_INSTALL_PREFIX}/share/doc)
endif()
# ----------------------------------------------------------------------

# ----------------------------------------------------------------------
include(FeatureSummary)
FEATURE_SUMMARY(WHAT ALL)
# ----------------------------------------------------------------------
