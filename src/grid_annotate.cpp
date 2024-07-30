#include "grid_annotate.h"
#include "grid_algo.h"

#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <unordered_map>
#include <vector>

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

using Contenders = std::vector<const Candidates*>;
using CandidateContenders = std::unordered_map<Digit, Contenders>;

CandidateContenders GetCandidateContenders(std::ranges::viewable_range auto range)
{
  CandidateContenders candidateContenders;

  for(auto&& candidates : range) {
    for(auto candidate : candidates) {
      auto contenders = candidateContenders.find(candidate);
      if(contenders == end(candidateContenders)) {
        contenders = candidateContenders.insert(std::make_pair(candidate, Contenders{})).first;
//        contenders->second.reserve(range.size());
      }
      contenders->second.push_back(&candidates);
    }
  }

  return candidateContenders;
}

struct GridCandidateContenders : public GridBase<CandidateContenders>
{
  GridCandidateContenders(const GridCandidates& gridCandidates)
  : GridBase<CandidateContenders>(gridCandidates.height(), gridCandidates.width(), gridCandidates.blockHeight, gridCandidates.blockHeight)
  {
    // create empty map entries
    (void)std::transform(
      gridCandidates.begin(), gridCandidates.end(),
      this->begin(),
      [](const Candidates& candidates) -> CandidateContenders {
        CandidateContenders candidateContenders;
        (void)std::transform(
          candidates.begin(), candidates.end(),
          std::inserter(candidateContenders, candidateContenders.end()),
          [](Digit d) -> CandidateContenders::value_type {
            return std::make_pair(d, Contenders{});
          });
        return candidateContenders;
      });

    // (over)fill map entries
    ForEachGroup(
      gridCandidates,
      std::views::zip(gridCandidates, *this),
      [](std::ranges::viewable_range auto range) {
        auto groupCandidateContenders = GetCandidateContenders(range | std::views::elements<0>);

        for(auto&& fieldCandidateContenders : range | std::views::elements<1>) {
          for(auto&& [candidate, fieldContenders] : fieldCandidateContenders) {
            if(auto groupContenders = groupCandidateContenders.find(candidate); groupContenders != groupCandidateContenders.end()) {
              fieldContenders.insert(fieldContenders.end(), groupContenders->second.begin(), groupContenders->second.end());
            }
          }
        }
      });

    // remove duplicate map entries that were found in row/colum and block
    for(auto&& candidateContenders : *this) {
      for(auto&& [_, contenders] : candidateContenders) {
        std::sort(contenders.begin(), contenders.end());

        contenders.erase(
          std::unique(contenders.begin(), contenders.end()),
          contenders.end());
      }
    }
  }
};

} // namespace anonymous

GridCandidates::GridCandidates() = default;

GridCandidates::GridCandidates(const Grid& grid)
: GridBase<Candidates>(grid.height(), grid.width(), grid.blockHeight, grid.blockHeight)
{
  // sorted list of all possible Sudoku digits
  auto elements = GetElements(grid);

  // add all elements as candidates to unsolved fields
  (void)std::transform(
    grid.begin(), grid.end(),
    this->begin(),
    [&elements](const Field& f) -> Candidates {
      if(!f.HasValue()) {
        return elements;
      }
      return Candidates{};
    });

  // trim candidates according to Sudoku constraints
  ForEachGroup(
    grid,
    std::views::zip(grid, *this),
    TrimCandidates{elements});
}

void OrderCandidates(GridCandidates& gridCandidates)
{
  auto gridCandidateContenders = GridCandidateContenders(gridCandidates);

  // sort the candidates by uniqueness
  for(auto&& [candidates, candidateContenders] : std::views::zip(gridCandidates, gridCandidateContenders)) {
    std::sort(
      begin(candidates), end(candidates),
      [&candidateContenders](Digit lhs, Digit rhs) -> bool {
        return (candidateContenders.at(lhs).size() < candidateContenders.at(rhs).size());
      });
  }
}
