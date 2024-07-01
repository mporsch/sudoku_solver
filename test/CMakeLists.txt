add_executable(test_grid_parse test_grid_parse.cpp)
target_link_libraries(test_grid_parse PRIVATE sudoku_grid)

add_executable(test_grid_check test_grid_check.cpp)
target_link_libraries(test_grid_check PRIVATE sudoku_grid)

enable_testing()
add_test(NAME test_grid_parse COMMAND test_grid_parse)
add_test(NAME test_grid_check COMMAND test_grid_check)

set(test_files
  reference.txt
  reference_solved.txt
  reference_6x6,3x2.txt
  reference_6x6,3x2_solved.txt
  reference_12x12,4x3.txt
)
foreach(test_file ${test_files})
  add_test(NAME "sudoku_solver:${test_file}" COMMAND sudoku_solver "${test_file}" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endforeach()

add_custom_target(build_tests DEPENDS
  test_grid_parse
  test_grid_check
  sudoku_solver
)

add_custom_target(run_tests
  COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION> --verbose
  DEPENDS build_tests
)
