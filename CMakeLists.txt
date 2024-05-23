cmake_minimum_required(VERSION 3.1.0)

project(sudoku_solver CXX)

find_package(hypervector REQUIRED)

add_executable(sudoku_solver
  sudoku_solver.cpp
)
target_link_libraries(sudoku_solver hypervector)
