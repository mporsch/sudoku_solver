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
  src/grid.h
  src/grid_check.h
  src/grid_check_impl.h
  src/grid_parse.h
  src/grid_print.h
)
target_include_directories(sudoku_grid PUBLIC src)
target_compile_features(sudoku_grid PUBLIC cxx_std_23) # for std::views::stride
target_link_libraries(sudoku_grid
  PUBLIC hypervector
  PRIVATE foonathan::lexy::ext foonathan::lexy::core
)

add_library(sudoku_solver_args
  src/args_parse.cpp
  src/args.h
  src/args_parse.h
)
target_compile_features(sudoku_solver_args PUBLIC cxx_std_17) # for std::optional
target_link_libraries(sudoku_solver_args
  PRIVATE foonathan::lexy::ext foonathan::lexy::core
)

add_executable(sudoku_solver
  src/sudoku_solver.cpp
)
target_link_libraries(sudoku_solver PRIVATE sudoku_solver_args sudoku_grid)

add_subdirectory(test)
