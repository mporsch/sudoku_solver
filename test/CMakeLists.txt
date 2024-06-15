add_executable(test_grid_check test_grid_check.cpp)
target_link_libraries(test_grid_check PRIVATE sudoku_grid)

enable_testing()
add_test(NAME test_grid_check COMMAND test_grid_check)
add_test(NAME sudoku_solver_reference COMMAND sudoku_solver reference.txt WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME sudoku_solver_reference_solved COMMAND sudoku_solver reference_solved.txt WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(build_tests DEPENDS
  test_grid_check
  sudoku_solver
)

add_custom_target(run_tests
  COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION> --verbose
  DEPENDS build_tests
)
