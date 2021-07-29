include(${PROJECT_UTILS_CMAKE_DIR}/doctest_pytest.cmake)


file(GLOB_RECURSE test_files
  CONFIGURE_DEPENDS "${src_dir}/*.test.cpp"
)

create_doctest(
  TESTED_TARGET ${PROJECT_NAME}
  LABEL         unit_tests
  SOURCES       ${PROJECT_UTILS_DIR}/src/unit_test_main.cpp ${test_files}
  SERIAL_RUN    true
  N_PROC        1
)
#target_link_libraries(${PROJECT_NAME}_doctest_unit_tests
#  PRIVATE
#    pybind11::embed
#)
