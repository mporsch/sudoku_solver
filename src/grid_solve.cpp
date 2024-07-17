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

Order GetOrder(Grid& grid)
{
  // get iterators to all fields
  auto order = Order(grid.size());
  std::iota(begin(order), end(order), grid.begin());

  // filter out the iterators to already solved fields
  (void)order.erase(
    std::remove_if(
      begin(order), end(order),
      [&](auto it) -> bool { return it->HasValue(); }
    ),
    end(order)
  );

  // sort iterators by number of unsolved field candidates
  std::sort(begin(order), end(order), CompareNumberOfCandidates{});

  return order;
}

bool Solve(
  Grid& grid,
  Order::iterator curr,
  Order::iterator last)
{
  // check if this branch can be solved
  if(!IsSolvable(grid)) {
    return false;
  }

  // the grid is solved if there are no more unsolved candidates
  if(curr == last) {
    std::cout << grid;
    return true;
  }

  // the unsolved field to attempt in this iteration
  auto&& field = *curr;

  // the next unsolved field to check after this one
  auto next = std::next(curr);

  // iterate the unsolved field's candidates
  assert(field->candidates.has_value());
  for(auto candidate : *field->candidates) {
    // try a candidate
    field->digit = candidate;

    // step into a branch based on this modification
    if(Solve(grid, next, last)) {
      return true;
    }
  }

  // revert our failed change and give up on this branch
  field->digit = Field::undef;
  return false;
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

  // do recursive solve iterations
  return Solve(grid, begin(order), end(order));
}
