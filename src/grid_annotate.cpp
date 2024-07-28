#include "grid_annotate.h"
#include "grid_algo.h"

#include <algorithm>
#include <iterator>
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

struct GridCandidateCounts : public GridBase<CandidateCounts>
{
  GridCandidateCounts(const GridCandidates& grid)
  : GridBase<CandidateCounts>(grid.height(), grid.width(), grid.blockHeight, grid.blockHeight)
  {
  }
};

GridCandidateCounts GetGridCandidateCounts(
  Grid& grid,
  GridCandidates& gridCandidates)
{
  // create with appropriate size
  auto gridCandidateCounts = GridCandidateCounts(grid);

  // create empty map entries
  (void)std::transform(
    gridCandidates.begin(), gridCandidates.end(),
    gridCandidateCounts.begin(),
    [](const Candidates& candidates) -> CandidateCounts {
      CandidateCounts candidateCounts;
      (void)std::transform(
        begin(candidates), end(candidates),
        std::inserter(candidateCounts, end(candidateCounts)),
        [](Digit d) -> CandidateCounts::value_type {
          return std::make_pair(d, CandidateCount{});
        });
      return candidateCounts;
    });

  // (over)fill map entries
  ForEachGroup(
    grid,
    std::views::zip(grid, gridCandidates, gridCandidateCounts),
    [](std::ranges::viewable_range auto range) {
      auto groupCandidateCounts = GetCandidateCounts(
        range
        | std::views::transform([](auto t) -> std::tuple<Field&, const Candidates&> {
          return {std::get<0>(t), std::get<1>(t)};
        }));

      for(auto&& fieldCandidateCounts : range | std::views::elements<2>) {
        for(auto&& [candidate, candidateCounts] : fieldCandidateCounts) {
          if(auto it = groupCandidateCounts.find(candidate); it != end(groupCandidateCounts)) {
            candidateCounts.insert(end(candidateCounts), begin(it->second), end(it->second));
          }
        }
      }
    });

  // remove duplicate map entries
  for(auto&& candidateCounts : gridCandidateCounts) {
    for(auto&& [_, candidateCount] : candidateCounts) {
      std::sort(begin(candidateCount), end(candidateCount));

      candidateCount.erase(
        std::unique(begin(candidateCount), end(candidateCount)),
        end(candidateCount));
    }
  }

  return gridCandidateCounts;
}

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

void OrderCandidates(
  Grid& grid,
  GridCandidates& gridCandidates)
{
  auto gridCandidateCounts = GetGridCandidateCounts(grid, gridCandidates);

  // sort the candidates by uniqueness
  for(auto&& [candidates, candidateCounts] : std::views::zip(gridCandidates, gridCandidateCounts)) {
    std::sort(
      begin(candidates), end(candidates),
      [&candidateCounts](Digit lhs, Digit rhs) -> bool {
        return (candidateCounts.at(lhs).size() < candidateCounts.at(rhs).size());
      });
  }
}
