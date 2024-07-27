#include "grid_annotate.h"
#include "grid_algo.h"

#include <algorithm>
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

Candidates Difference(
  const Candidates& c1,
  const Candidates& c2)
{
  Candidates diff;
  diff.reserve(c1.size());
  (void)std::ranges::set_difference(
    c1, c2,
    std::back_inserter(diff));
  return diff;
}

Candidates Intersection(
  const Candidates& c1,
  const Candidates& c2)
{
  Candidates common;
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

struct TrimCandidates
{
  const Digits& elements;

  void operator()(std::ranges::viewable_range auto range) const
  {
    // clues, i.e. the fields that already have a value
    auto givens = UniqueDigits(To<Digits>(range | std::views::elements<0>));

    // the candidates of this group are all elements that are not givens in it
    auto groupCandidates = Difference(elements, givens);

    for(std::tuple<const Field&, Candidates&> t : range) {
      auto&& [field, fieldCandidates] = t;

      if(!field.HasValue()) {
        // keep only the field's candidates that are candidates of this group
        fieldCandidates = Intersection(fieldCandidates, groupCandidates);
      }
    }
  }
};

} // namespace anonymous

GridCandidates::GridCandidates() = default;

GridCandidates::GridCandidates(const Grid& grid)
: GridBase<Candidates>(grid.height(), grid.width(), grid.blockHeight, grid.blockHeight)
{
}

GridCandidates Annotated(const Grid& grid)
{
  auto gridCandidates = GridCandidates(grid);

  // sorted list of all possible Sudoku digits
  auto elements = GetElements(grid);

  // add all elements as candidates to unsolved fields
  (void)std::transform(
    grid.begin(), grid.end(),
    gridCandidates.begin(),
    [&elements](const Field& f) -> Candidates {
      if(!f.HasValue()) {
        return elements;
      }
      return Candidates{};
    });

  // trim candidates according to Sudoku constraints
  ForEachGroup(
    grid,
    std::views::zip(grid, gridCandidates),
    TrimCandidates{elements});

  return gridCandidates;
}
