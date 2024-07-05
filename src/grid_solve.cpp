#include "grid_solve.h"
#include "grid_check.h"
#include "grid_print.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace {

// i.e. the alphabet of the Sudoku; 1-9 in most cases but not necessarily
using Elements = Grid::container;

Elements GetElements(const Grid& grid)
{
  auto fields = Elements(grid.begin(), grid.end());
  std::sort(begin(fields), end(fields));

  // trim empty fields
  fields.erase(
    begin(fields),
    std::find_if(begin(fields), end(fields), [](const Field& f) { return f != Field(); })
  );

  // remove duplicates
  fields.erase(std::unique(begin(fields), end(fields)), end(fields));

  // fill up if we don't have enough values yet
  while(fields.size() < grid.blockWidth * grid.blockHeight) {
    // this is dumb, but we can only easily make up some yet-unused values
    fields.push_back(fields.back().num + 1);
  }

  if(fields.size() != grid.blockWidth * grid.blockHeight) {
    throw std::invalid_argument("more field values than block size");
  }

  return fields;
}

IsSolved Solve(
  Grid &grid,
  Grid::iterator mid,
  const Elements& elements)
{
  // check if this branch is or can even be solved
  switch(auto isSolved = Check(grid)) {
    case IsSolved::Yes:
      std::cout << grid;
      [[fallthrough]];
    case IsSolved::Never:
      return isSolved;
    default:
      break;
  }

  // find the next position to write
  mid = std::find(mid, grid.end(), Field());
  if(mid == grid.end()) {
    throw std::logic_error("unexpected");
  }

  // the next field to check is after the one just written
  auto next = std::next(mid);

  for(auto&& field : elements) {
    // try a candidate element
    *mid = field;

    // step into a branch based on this modification
    switch(Solve(grid, next, elements)) {
      case IsSolved::Yes:
        return IsSolved::Yes;
      default:
        break;
    }
  }

  // revert our failed change and give up on this branch
  *mid = Field();
  return IsSolved::Never;
}

} // namespace anonymous

bool Solve(Grid grid)
{
  switch(Solve(grid, grid.begin(), GetElements(grid))) {
    case IsSolved::Yes:
      return true;
    default:
      return false;
  }
}
