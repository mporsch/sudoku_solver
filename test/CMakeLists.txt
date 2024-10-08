add_executable(test_grid_parse test_grid_parse.cpp)
target_link_libraries(test_grid_parse PRIVATE sudoku_grid_io)

add_executable(test_grid_algo test_grid_algo.cpp)
target_link_libraries(test_grid_algo PRIVATE sudoku_grid_algo)

add_executable(test_grid_annotate test_grid_annotate.cpp)
target_compile_features(test_grid_annotate PRIVATE cxx_std_23) # for std::views::zip
target_link_libraries(test_grid_annotate PRIVATE sudoku_grid_annotate)

add_executable(test_grid_solve test_grid_solve.cpp)
target_link_libraries(test_grid_solve PRIVATE sudoku_grid_solve)

enable_testing()
add_test(NAME test_grid_parse COMMAND test_grid_parse)
add_test(NAME test_grid_algo COMMAND test_grid_algo)
add_test(NAME test_grid_annotate COMMAND test_grid_annotate)
add_test(NAME test_grid_solve COMMAND test_grid_solve)

set(test_args
  --template
  --template=2
  --template=2x3
  --template=2,2
  --template=2x3,2
  --template=2,2x2
  --template=2x3,2x3
  reference.txt
  reference_solved.txt
  reference_4x4,2x2.txt
  reference_4x4,2x2_solved.txt
  reference_6x6,3x2.txt
  reference_6x6,3x2_solved.txt
  reference_12x12,4x3.txt
)
foreach(test_arg ${test_args})
  add_test(NAME "sudoku_solver:${test_arg}" COMMAND sudoku_solver "${test_arg}" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endforeach()

add_custom_target(build_tests DEPENDS
  test_grid_parse
  test_grid_algo
  test_grid_annotate
  test_grid_solve
  sudoku_solver
)

add_custom_target(run_tests
  COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION> --verbose
  DEPENDS build_tests
)
