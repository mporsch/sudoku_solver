#include "grid_solve.h"

#include <cstdlib>

Grid ReferenceGrid()
{
  auto grid = Grid{
    {0, 3, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 9, 5, 0, 0, 0},
    {0, 0, 8, 0, 0, 0, 0, 6, 0},
    {8, 0, 9, 7, 6, 1, 4, 2, 3},
    {4, 0, 6, 8, 5, 3, 7, 9, 1},
    {0, 0, 3, 9, 2, 4, 8, 5, 6},
    {0, 6, 1, 5, 3, 7, 2, 8, 4},
    {0, 0, 7, 4, 1, 9, 6, 3, 5},
    {0, 0, 5, 2, 8, 6, 1, 7, 9}};
  grid.blockWidth = 3;
  grid.blockHeight = 3;
  return grid;
}

Grid ReferenceGrid6x6_3x2()
{
  auto grid = Grid{
    {1, 0, 0, 0, 4, 0},
    {5, 0, 0, 2, 0, 0},
    {0, 4, 1, 0, 0, 0},
    {0, 0, 0, 4, 6, 0},
    {0, 0, 5, 0, 0, 4},
    {0, 6, 0, 0, 0, 3}};
  grid.blockWidth = 3;
  grid.blockHeight = 2;
  return grid;
}

int main(int, char**)
{
  return (true
  && Solve(ReferenceGrid6x6_3x2())
  && Solve(ReferenceGrid())
  ? EXIT_SUCCESS : EXIT_FAILURE);
}
