#include "grid_annotate.h"
#include "grid_algo.h"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <ranges>
#include <stdexcept>

namespace {

Digits UniqueDigits(Digits digits)
{
  std::sort(begin(digits), end(digits));

  // trim unsolved
  (void)digits.erase(
    begin(digits),
    std::find_if(
      begin(digits), end(digits),
      [](auto d) { return (d != Field::undef); }
    )
  );

  // remove duplicates
  (void)digits.erase(
    std::unique(begin(digits), end(digits)),
    end(digits)
  );

  return digits;
}

Field::Candidates Difference(
  const Field::Candidates& c1,
  const Field::Candidates& c2)
{
  Field::Candidates diff;
  diff.reserve(c1.size());
  (void)std::ranges::set_difference(
    c1, c2,
    std::back_inserter(diff));
  return diff;
}

Field::Candidates Intersection(
  const Field::Candidates& c1,
  const Field::Candidates& c2)
{
  Field::Candidates common;
  common.reserve(c1.size());
  (void)std::ranges::set_intersection(
    c1, c2,
    std::back_inserter(common));
  return common;
}

// the *whole* alphabet of the Sudoku; 1-9 in most cases but not necessarily
Digits GetElements(const Grid& grid)
{
  auto elements = UniqueDigits(Digits(grid.begin(), grid.end()));

  // fill up if we don't have enough values yet
  while(elements.size() < grid.blockWidth * grid.blockHeight) {
    // this is dumb, but we can only easily make up some yet-unused values
    elements.push_back(elements.back() + 1);
  }

  if(elements.size() != grid.blockWidth * grid.blockHeight) {
    throw std::invalid_argument("more elements than block size");
  }

  return elements;
}

struct AnnotateCandidates
{
  const Digits& elements;

  void operator()(std::ranges::viewable_range auto&& range) const
  {
    // clues, i.e. the fields that already have a value
    auto givens = UniqueDigits(To<Digits>(range));

    // the candidates of this group are all elements that are not givens in it
    auto groupCandidates = Difference(elements, givens);

    for(auto&& field : range) {
      if(!field.HasValue()) {
        assert(field.candidates.has_value());
        auto&& fieldCandidates = *field.candidates;

        // keep only the field's candidates that are candidates of this group
        fieldCandidates = Intersection(fieldCandidates, groupCandidates);
      }
    }
  }
};

} // namespace anonymous

void Annotate(Grid& grid)
{
  // sorted list of all possible Sudoku digits
  auto elements = GetElements(grid);

  // add all elements as candidates to unsolved fields
  for(auto&& f : grid) {
    if(!f.HasValue()) {
      f.candidates.emplace(elements);
    }
  }

  // trim candidates according to Sudoku constraints
  ForEachGroup(grid, AnnotateCandidates{elements});
}

Grid Annotated(Grid grid)
{
  Annotate(grid);
  return grid;
}
