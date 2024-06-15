cmake_minimum_required(VERSION 3.1.0)

project(sudoku_solver CXX)

find_package(hypervector REQUIRED HINTS submodules/hypervector)

include(FetchContent)
FetchContent_Declare(lexy URL https://github.com/foonathan/lexy/releases/download/v2022.12.1/lexy-src.zip)
FetchContent_MakeAvailable(lexy)

add_library(sudoku_grid
  src/grid_check.cpp
  src/grid_parse.cpp
  src/grid_print.cpp
  src/custom_step_iterator.h
  src/dance_dance_iterator.h
  src/grid.h
  src/grid_check.h
  src/grid_check_impl.h
  src/grid_parse.h
  src/grid_print.h
  src/preserve_iterator.h
)
target_compile_features(sudoku_grid PUBLIC cxx_std_17) # for std::optional
target_link_libraries(sudoku_grid PUBLIC hypervector foonathan::lexy::ext foonathan::lexy::core)

add_executable(sudoku_solver
  src/args_parse.cpp
  src/sudoku_solver.cpp
  src/args.h
  src/args_parse.h
)
target_link_libraries(sudoku_solver PRIVATE sudoku_grid)


add_executable(test_grid_check
  src/test_grid_check.cpp
)
target_link_libraries(test_grid_check PRIVATE sudoku_grid)

enable_testing()
add_test(NAME test_grid_check COMMAND test_grid_check WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_custom_target(build_tests
  DEPENDS test_grid_check
)

add_custom_target(run_tests
  COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION> --verbose
  DEPENDS build_tests
)
