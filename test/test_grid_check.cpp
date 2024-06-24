#include "grid_check_impl.h"
#include "grid.h"

#include <cstdlib>
#include <deque>

struct CheckExpected
{
  bool operator()(Grid::container sorted)
  {
    return GetNextRange() == sorted;
  }

  Grid::container GetNextRange()
  {
    // the ranges to check
    static auto ranges = std::deque<Grid::container>{
      // field rows
      {5, 3, 4, 6, 7, 8, 9, 1, 2},
      {6, 7, 2, 1, 9, 5, 3, 4, 8},
      {1, 9, 8, 3, 4, 2, 5, 6, 7},
      {8, 5, 9, 7, 6, 1, 4, 2, 3},
      {4, 2, 6, 8, 5, 3, 7, 9, 1},
      {7, 1, 3, 9, 2, 4, 8, 5, 6},
      {9, 6, 1, 5, 3, 7, 2, 8, 4},
      {2, 8, 7, 4, 1, 9, 6, 3, 5},
      {3, 4, 5, 2, 8, 6, 1, 7, 9},
      // field columns
      {5, 6, 1, 8, 4, 7, 9, 2, 3},
      {3, 7, 9, 5, 2, 1, 6, 8, 4},
      {4, 2, 8, 9, 6, 3, 1, 7, 5},
      {6, 1, 3, 7, 8, 9, 5, 4, 2},
      {7, 9, 4, 6, 5, 2, 3, 1, 8},
      {8, 5, 2, 1, 3, 4, 7, 9, 6},
      {9, 3, 5, 4, 7, 8, 2, 6, 1},
      {1, 4, 6, 2, 9, 5, 8, 3, 7},
      {2, 8, 7, 3, 1, 6, 4, 5, 9},
      // blocks by row and column
      {5, 3, 4, 6, 7, 2, 1, 9, 8},
      {6, 7, 8, 1, 9, 5, 3, 4, 2},
      {9, 1, 2, 3, 4, 8, 5, 6, 7},
      {8, 5, 9, 4, 2, 6, 7, 1, 3},
      {7, 6, 1, 8, 5, 3, 9, 2, 4},
      {4, 2, 3, 7, 9, 1, 8, 5, 6},
      {9, 6, 1, 2, 8, 7, 3, 4, 5},
      {5, 3, 7, 4, 1, 9, 2, 8, 6},
      {2, 8, 4, 6, 3, 5, 1, 7, 9}};

    auto range = std::move(ranges.front());
    ranges.pop_front();
    return range;
  }
};

Grid ReferenceGrid()
{
  return Grid{
    {5, 3, 4, 6, 7, 8, 9, 1, 2},
    {6, 7, 2, 1, 9, 5, 3, 4, 8},
    {1, 9, 8, 3, 4, 2, 5, 6, 7},
    {8, 5, 9, 7, 6, 1, 4, 2, 3},
    {4, 2, 6, 8, 5, 3, 7, 9, 1},
    {7, 1, 3, 9, 2, 4, 8, 5, 6},
    {9, 6, 1, 5, 3, 7, 2, 8, 4},
    {2, 8, 7, 4, 1, 9, 6, 3, 5},
    {3, 4, 5, 2, 8, 6, 1, 7, 9}};
}

int main()
{
  auto grid = ReferenceGrid();
  return Checker<CheckExpected>{grid} ? EXIT_SUCCESS : EXIT_FAILURE;
}