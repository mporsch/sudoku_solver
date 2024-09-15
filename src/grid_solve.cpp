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
#include <vector>

namespace {

struct ContenderField
{
  Field* field;

  ContenderField(std::tuple<const Candidates&, Field&> t)
  : field(&std::get<Field&>(t))
  {
  }
};

using CandidateContendersHiddenSingles = CandidateContenders<ContenderField>;

CandidateContendersHiddenSingles TrimCandidateContenders(
  std::ranges::viewable_range auto range,
  CandidateContendersHiddenSingles candidateContenders)
{
  for(auto it = begin(candidateContenders); it != end(candidateContenders);) {
    auto found = std::ranges::contains(
      range | std::views::elements<1>,
      it->first,
      &Field::digit);
    if(found) {
      it = candidateContenders.erase(it); // a previous iteration already solved that one -> trim
    } else {
      ++it;
    }
  }
  return candidateContenders;
}

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
  const GridCandidates& gridCandidates)
{
  bool found = false;

  for(auto&& [field, candidates] : std::views::zip(grid, gridCandidates)) {
    if(candidates.size() == 1) {
      // "Single" (or singleton, or lone number): The only candidate in a cell
      field.digit = candidates.front();

      found = true;
    }
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
    std::views::zip(gridCandidates, grid),
    [&](std::ranges::viewable_range auto range) {
      auto candidateContenders = TrimCandidateContenders(
        range,
        GetCandidateContenders<ContenderField, const Candidates&, Field&>(range));

      for(auto&& [candidate, contenders] : candidateContenders) {
        if(contenders.size() == 1) {
          auto&& field = *contenders.front().field;

          // "Hidden single": A candidate that appears with others, but only once in a given row, column or box
          assert(!field.HasValue() || field.digit == candidate);
          field.digit = candidate;

          found = true;
        }
      }
    }
  );

  return found;
}

bool SolveDeterministic(Grid grid, bool verbose = false);

bool SolveTrialAndError(Grid grid, GridCandidates gridCandidates)
{
  Idx idx;
  {
    // get indices of unsolved fields, sorted by number of candidates
    auto order = GetOrder(grid, gridCandidates);

    // the grid is solved if there are no more unsolved candidates
    if(order.empty()) {
      std::cerr << "\n\n";
      std::cout << grid;
      return true;
    }

    idx = order.front();
  }

  // sort the candidates by uniqueness
  OrderCandidates(gridCandidates);

  // the unsolved field to attempt in this iteration
  auto&& field = *std::next(grid.begin(), idx);
  auto&& candidates = *std::next(gridCandidates.begin(), idx);

  // iterate the unsolved field's candidates
  for(auto candidate : candidates) {
    // try a candidate
    field.digit = candidate;

    // check if this branch can be solved and step into a branch based on this modification
    if(IsSolvable(grid) && SolveDeterministic(grid)) {
      return true;
    }
  }

  // give up on this branch
  return false;
}

bool SolveDeterministic(Grid grid, bool verbose)
{
  if(verbose) {
    std::cerr<< "\n\nSolving deterministically...";
  }

  GridCandidates gridCandidates;
  for(bool found = true; found;) {
    // annotate the unsolved fields with their candidates
    gridCandidates = GridCandidates(grid);

    found = false
    || SolveSingles(grid, gridCandidates)
    || SolveHiddenSingles(grid, gridCandidates);

    // check if this branch can be solved or if we reached a dead end
    if(!IsSolvable(grid)) {
      return false;
    }
  }

  if(verbose) {
    std::cerr
      << "\n\n"
      << grid
      << "\n\nSolving recursively with trial and error...";
  }

  return SolveTrialAndError(
    std::move(grid),
    std::move(gridCandidates));
}

} // namespace anonymous

bool Solve(Grid grid)
{
  // do recursive solve iterations
  return SolveDeterministic(std::move(grid), true);
}
