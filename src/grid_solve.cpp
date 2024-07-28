#include "grid_solve.h"
#include "grid_algo.h"
#include "grid_annotate.h"
#include "grid_check.h"
#include "grid_print.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <ranges>

namespace {

using Idx = Grid::iterator::difference_type;
using Order = std::vector<Idx>;

struct CompareNumberOfCandidates
{
  const GridCandidates& grid;

  bool operator()(Idx lhs, Idx rhs) const
  {
    return (std::next(grid.begin(), lhs)->size() < std::next(grid.begin(), rhs)->size());
  }
};

Order GetOrder(
  const Grid& grid,
  const GridCandidates& gridCandidates)
{
  // get indices of all fields
  auto order = Order(grid.size());
  std::iota(begin(order), end(order), static_cast<Idx>(0));

  // filter out the indices of already solved fields
  (void)order.erase(
    std::remove_if(
      begin(order), end(order),
      [&](Idx idx) -> bool { return std::next(grid.begin(), idx)->HasValue(); }
    ),
    end(order)
  );

  // sort indices by number of unsolved field candidates
  std::sort(begin(order), end(order), CompareNumberOfCandidates{gridCandidates});

  return order;
}

bool SolveSingles(
  Grid& grid,
  const GridCandidates& gridCandidates,
  Order::iterator curr,
  Order::iterator last)
{
  bool found = false;
  for(; (curr != last) && (std::next(gridCandidates.begin(), *curr)->size() == 1); ++curr) {
    // "Single" (or singleton, or lone number) – The only candidate in a cell
    std::next(grid.begin(), *curr)->digit = std::next(gridCandidates.begin(), *curr)->front();

    found = true;
  }
  return found;
}

bool SolveHiddenSingles(
  Grid& grid,
  const GridCandidates& gridCandidates)
{
  bool found = false;

  ForEachGroup(
    grid,
    std::views::zip(grid, gridCandidates),
    [&](std::ranges::viewable_range auto range) {
      auto candidateCounts = GetCandidateCounts(std::move(range));

      for(auto&& [candidate, candidateCount] : candidateCounts) {
        if(candidateCount.size() == 1) {
          auto&& field = *candidateCount.front();

          // "Hidden single" – A candidate that appears with others, but only once in a given row, column or box
          assert(!field.HasValue() || field.digit == candidate);
          field.digit = candidate;

          found = true;
        }
      }
    }
  );

  return found;
}

bool Solve(
  Grid& grid,
  const GridCandidates& gridCandidates,
  Order::iterator curr,
  Order::iterator last)
{
  // check if this branch can be solved
  if(!IsSolvable(grid)) {
    return false;
  }

  // the grid is solved if there are no more unsolved candidates
  if(curr == last) {
    std::cout << "\n\n" << grid;
    return true;
  }

  // the unsolved field to attempt in this iteration
  auto&& field = *std::next(grid.begin(), *curr);
  auto&& candidates = *std::next(gridCandidates.begin(), *curr);

  // the next unsolved field to check after this one
  auto next = std::next(curr);

  // iterate the unsolved field's candidates
  for(auto candidate : candidates) {
    // try a candidate
    field.digit = candidate;

    // step into a branch based on this modification
    if(Solve(grid, gridCandidates, next, last)) {
      return true;
    }
  }

  // revert our failed change and give up on this branch
  field.digit = Field::undef;
  return false;
}

} // namespace anonymous

bool Solve(Grid grid)
{
  std::cerr<< "\n\nSolving algorithmically...";

  GridCandidates gridCandidates;
  Order order;

  for(bool found = true; found;) {
    // annotate the unsolved fields with their candidates
    gridCandidates = Annotated(grid);

    found = SolveHiddenSingles(grid, gridCandidates);
    if(found) {
      continue;
    }

    // get iterators to unsolved fields, sorted by number of candidates
    order = GetOrder(grid, gridCandidates);

    found = SolveSingles(grid, gridCandidates, begin(order), end(order));
  }

  std::cerr
    << "\n\n"
    << grid
    << "\n\nSolving with trial and error...";

  // do recursive solve iterations
  return Solve(grid, gridCandidates, begin(order), end(order));
}
