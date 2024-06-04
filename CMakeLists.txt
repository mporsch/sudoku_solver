cmake_minimum_required(VERSION 3.1.0)

project(sudoku_solver CXX)

find_package(hypervector REQUIRED HINTS repos/hypervector)

include(FetchContent)
FetchContent_Declare(lexy URL https://github.com/foonathan/lexy/releases/download/v2022.12.1/lexy-src.zip)
FetchContent_MakeAvailable(lexy)

add_executable(sudoku_solver
  args_parse.cpp
  sudoku_solver.cpp
  args.h
  args_parse.h
  grid.h
  grid_parse.h
  grid_print.h
)
target_compile_features(sudoku_solver PRIVATE cxx_std_20) # for std::string_view::starts_with
target_link_libraries(sudoku_solver PRIVATE hypervector foonathan::lexy::ext foonathan::lexy::core)
