#include "grid_check_impl.h"
#include "grid.h"

#include <cstdlib>
#include <deque>

template<typename GetReferenceGroups>
struct CheckExpected
{
  bool operator()(Grid::container sorted)
  {
    return GetNextRange() == sorted;
  }

  Grid::container GetNextRange()
  {
    // the groups to check
    static std::deque<Grid::container> groups = GetReferenceGroups();

    auto group = std::move(groups.front());
    groups.pop_front();

    for(auto&& f : group) {
      if(f.num) {
        f.num += 48; // upshift from integer to ASCII
      }
    }

    return group;
  }
};

struct ReferenceGroups
{
  operator std::deque<Grid::container>() const
  {
    return std::deque<Grid::container>{
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
  }
};

struct ReferenceGroups6x6_3x2
{
  operator std::deque<Grid::container>() const
  {
    return std::deque<Grid::container>{
      // field rows
      {1, 2, 6, 3, 4, 5},
      {5, 3, 4, 2, 1, 6},
      {6, 4, 1, 5, 3, 2},
      {2, 5, 3, 4, 6, 1},
      {3, 1, 5, 6, 2, 4},
      {4, 6, 2, 1, 5, 3},
      // field columns
      {1, 5, 6, 2, 3, 4},
      {2, 3, 4, 5, 1, 6},
      {6, 4, 1, 3, 5, 2},
      {3, 2, 5, 4, 6, 1},
      {4, 1, 3, 6, 2, 5},
      {5, 6, 2, 1, 4, 3},
      // blocks by row and column
      {1, 2, 6, 5, 3, 4},
      {3, 4, 5, 2, 1, 6},
      {6, 4, 1, 2, 5, 3},
      {5, 3, 2, 4, 6, 1},
      {3, 1, 5, 4, 6, 2},
      {6, 2, 4, 1, 5, 3}};
  }
};

Grid ReferenceGrid()
{
  auto grid = Grid{
    {5, 3, 4, 6, 7, 8, 9, 1, 2},
    {6, 7, 2, 1, 9, 5, 3, 4, 8},
    {1, 9, 8, 3, 4, 2, 5, 6, 7},
    {8, 5, 9, 7, 6, 1, 4, 2, 3},
    {4, 2, 6, 8, 5, 3, 7, 9, 1},
    {7, 1, 3, 9, 2, 4, 8, 5, 6},
    {9, 6, 1, 5, 3, 7, 2, 8, 4},
    {2, 8, 7, 4, 1, 9, 6, 3, 5},
    {3, 4, 5, 2, 8, 6, 1, 7, 9}};
  grid.blockWidth = 3;
  grid.blockHeight = 3;
  return grid;
}

Grid ReferenceGrid6x6_3x2()
{
  auto grid = Grid{
    {1, 2, 6, 3, 4, 5},
    {5, 3, 4, 2, 1, 6},
    {6, 4, 1, 5, 3, 2},
    {2, 5, 3, 4, 6, 1},
    {3, 1, 5, 6, 2, 4},
    {4, 6, 2, 1, 5, 3}};
  grid.blockWidth = 3;
  grid.blockHeight = 2;
  return grid;
}

int main()
{
  return (true
  && Checker<CheckExpected<ReferenceGroups>>{ReferenceGrid()}
  && Checker<CheckExpected<ReferenceGroups6x6_3x2>>{ReferenceGrid6x6_3x2()}
  ? EXIT_SUCCESS : EXIT_FAILURE);
}
