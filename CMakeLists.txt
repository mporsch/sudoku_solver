cmake_minimum_required(VERSION 3.1.0)

project(sudoku_solver CXX)

find_package(hypervector REQUIRED HINTS submodules/hypervector)

include(FetchContent)
FetchContent_Declare(lexy URL https://github.com/foonathan/lexy/releases/download/v2022.12.1/lexy-src.zip)
FetchContent_MakeAvailable(lexy)

add_executable(sudoku_solver
  src/args_parse.cpp
  src/grid_parse.cpp
  src/grid_print.cpp
  src/sudoku_solver.cpp
  src/args.h
  src/args_parse.h
  src/grid.h
  src/grid_parse.h
  src/grid_print.h
)
target_compile_features(sudoku_solver PRIVATE cxx_std_20) # for std::string_view::starts_with
target_link_libraries(sudoku_solver PRIVATE hypervector foonathan::lexy::ext foonathan::lexy::core)
