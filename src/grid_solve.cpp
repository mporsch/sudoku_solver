#include "grid_solve.h"
#include "grid_algo.h"
#include "grid_check.h"
#include "grid_print.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <ranges>
#include <stdexcept>

namespace {

Grid::container UniqueDigits(Grid::container fields)
{
  std::sort(begin(fields), end(fields));

  // trim empty fields
  fields.erase(
    begin(fields),
    std::find_if(begin(fields), end(fields), [](const Field& f) { return f != Field(); })
  );

  // remove duplicates
  fields.erase(std::unique(begin(fields), end(fields)), end(fields));

  return fields;
}

// i.e. the alphabet of the Sudoku; 1-9 in most cases but not necessarily
using Elements = Grid::container;

Elements GetElements(const Grid& grid)
{
  auto fields = UniqueDigits(Elements(grid.begin(), grid.end()));

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

struct AnnotateCandidates
{
  const Elements& elements;

  void operator()(std::ranges::viewable_range auto&& range) const
  {
    auto givens = UniqueDigits(ToFields(range));

    // the candidates of this group are all elements that are not given in it
    Field::Candidates groupCandidates;
    (void)std::set_difference(
      begin(elements), end(elements),
      begin(givens), end(givens),
      std::inserter(groupCandidates, begin(groupCandidates)));

    for(auto&& field : range) {
      if(field == Field()) {
        assert(field.candidates.has_value());
        auto&& fieldCandidates = *field.candidates;

        // keep only the field's candidates that are candidates of this group
        (void)fieldCandidates.erase(
          std::remove_if(
            begin(fieldCandidates), end(fieldCandidates),
            [&](auto&& candidate) -> bool {
              auto it = std::find(begin(groupCandidates), end(groupCandidates), candidate);
              return it == end(groupCandidates);
            }
          ),
          end(fieldCandidates)
        );
      }
    }
  }
};

void Annotate(Grid& grid)
{
  auto elements = GetElements(grid);

  for(auto&& f : grid) {
    if(f == Field()) {
      f.candidates.emplace(begin(elements), end(elements));
    }
  }

  ForEachGroup(grid, AnnotateCandidates{elements});
}

IsSolved Solve(Grid &grid, Grid::iterator mid)
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

  assert(mid->candidates.has_value());
  for(auto&& candidate : *mid->candidates) {
    // try a candidate element
    mid->num = candidate;

    // step into a branch based on this modification
    switch(Solve(grid, next)) {
      case IsSolved::Yes:
        return IsSolved::Yes;
      default:
        break;
    }
  }

  // revert our failed change and give up on this branch
  mid->num = Field::undef;
  return IsSolved::Never;
}

} // namespace anonymous

bool Solve(Grid grid)
{
  Annotate(grid);

  switch(Solve(grid, grid.begin())) {
    case IsSolved::Yes:
      return true;
    default:
      return false;
  }
}
