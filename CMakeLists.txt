cmake_minimum_required(VERSION 3.1.0)

project(sudoku_solver CXX)

find_package(hypervector REQUIRED HINTS submodules/hypervector)

include(FetchContent)
FetchContent_Declare(lexy URL https://github.com/foonathan/lexy/releases/download/v2022.12.1/lexy-src.zip)
FetchContent_MakeAvailable(lexy)

add_library(sudoku_grid INTERFACE
  src/grid.h
)
target_include_directories(sudoku_grid INTERFACE src)
target_compile_features(sudoku_grid INTERFACE cxx_std_17) # for std::optional
target_link_libraries(sudoku_grid
  INTERFACE hypervector
)

add_library(sudoku_grid_io
  src/grid_parse.cpp
  src/grid_print.cpp
  src/grid_parse.h
  src/grid_print.h
)
target_link_libraries(sudoku_grid_io
  PUBLIC sudoku_grid
  PRIVATE foonathan::lexy::ext foonathan::lexy::core
)

add_library(sudoku_grid_algo INTERFACE
  src/grid_algo.h
)
target_compile_features(sudoku_grid_algo INTERFACE cxx_std_23) # for std::views::stride
target_link_libraries(sudoku_grid_algo
  INTERFACE sudoku_grid
)

add_library(sudoku_grid_check
  src/grid_check.cpp
  src/grid_check.h
)
target_link_libraries(sudoku_grid_check
  PUBLIC sudoku_grid
  PRIVATE sudoku_grid_algo
)

add_library(sudoku_grid_annotate
  src/grid_annotate.cpp
  src/grid_annotate.h
)
target_link_libraries(sudoku_grid_annotate
  PUBLIC sudoku_grid
  PRIVATE sudoku_grid_algo
)

add_library(sudoku_grid_solve
  src/grid_solve.cpp
  src/grid_solve.h
)
target_link_libraries(sudoku_grid_solve
  PUBLIC sudoku_grid
  PRIVATE sudoku_grid_check sudoku_grid_annotate sudoku_grid_io
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
target_link_libraries(sudoku_solver
  PRIVATE sudoku_solver_args sudoku_grid_solve sudoku_grid_io
)

add_subdirectory(test)
