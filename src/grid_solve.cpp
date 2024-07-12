#include "grid_solve.h"
#include "grid_annotate.h"
#include "grid_check.h"
#include "grid_print.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>

namespace {

using Order = std::vector<Grid::iterator>;

struct CompareNumberOfCandidates
{
  bool operator()(
    Grid::iterator lhs,
    Grid::iterator rhs) const
  {
    assert(lhs->candidates.has_value() && rhs->candidates.has_value());
    return (lhs->candidates->size() < rhs->candidates->size());
  }
};

Order GetOrder(Grid &grid)
{
  // get iterators to all fields
  auto order = Order(grid.size());
  std::iota(begin(order), end(order), grid.begin());

  // filter out the iterators to already solved fields
  (void)order.erase(
    std::remove_if(
      begin(order), end(order),
      [&](auto&& it) -> bool {
        return it->HasValue();
      }
    ),
    end(order)
  );

  // sort iterators by number of unsolved field candidates
  std::sort(begin(order), end(order), CompareNumberOfCandidates{});

  return order;
}

IsSolved Solve(
  Grid &grid,
  Order::iterator curr,
  Order::iterator last)
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

  // the unsolved field to attempt in this iteration
  auto&& field = *curr;

  // the next unsolved field to check after this one
  auto next = std::next(curr);

  // iterate the unsolved field's candidates
  assert(field->candidates.has_value());
  for(auto&& candidate : *field->candidates) {
    // try a candidate
    field->digit = candidate;

    // step into a branch based on this modification
    switch(Solve(grid, next, last)) {
      case IsSolved::Yes:
        return IsSolved::Yes;
      default:
        break;
    }
  }

  // revert our failed change and give up on this branch
  field->digit = Field::undef;
  return IsSolved::Never;
}

} // namespace anonymous

bool Solve(Grid grid)
{
  // annotate the unsolved fields with their candidates
  Annotate(grid);

  // TODO more preprocessing to solve:
  // - "Single" (or singleton, or lone number) – The only candidate in a cell
  // - "Hidden single" – A candidate that appears with others, but only once in a given row, column or box

  // get iterators to unsolved fields, sorted by number of candidates
  auto order = GetOrder(grid);

  // start recursive solve iterations, then examine the outcome
  switch(Solve(grid, begin(order), end(order))) {
    case IsSolved::Yes:
      return true;
    default:
      return false;
  }
}
