#include "grid_solve.h"
#include "grid_check.h"
#include "grid_print.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace {

using Alphabet = Grid::container;

Alphabet GetAlphabet(const Grid& grid)
{
  auto fields = Alphabet(grid.begin(), grid.end());
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
  const Alphabet& alphabet)
{
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

  for(auto&& f : alphabet) {
    *mid = f;

    switch(Solve(grid, next, alphabet)) {
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
  switch(Solve(grid, grid.begin(), GetAlphabet(grid))) {
    case IsSolved::Yes:
      return true;
    default:
      return false;
  }
}
