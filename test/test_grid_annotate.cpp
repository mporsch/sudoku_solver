#include "grid_annotate.h"

#include <algorithm>
#include <cstdlib>
#include <ranges>

Grid ReferenceGrid6x6_3x2()
{
  auto grid = Grid{
    {1, 0, 0, 0, 4, 0},
    {5, 0, 0, 2, 0, 0},
    {0, 4, 1, 0, 0, 0},
    {0, 0, 0, 4, 6, 0},
    {0, 0, 5, 0, 0, 4},
    {0, 6, 0, 0, 0, 3}};
  grid.blockWidth = 3;
  grid.blockHeight = 2;
  return grid;
}

std::vector<Candidates> ReferenceCandidates6x6_3x2()
{
  auto candidatesList = std::vector<Candidates>{
    {},
    {2, 3},
    {2, 3, 6},
    {3, 5, 6},
    {},
    {5, 6},
    {},
    {3},
    {3, 4, 6},
    {},
    {1, 3},
    {1, 6},
    {2, 3, 6},
    {},
    {},
    {3, 5},
    {2, 3, 5},
    {2, 5},
    {2, 3},
    {2, 3, 5},
    {2, 3},
    {},
    {},
    {1, 2, 5},
    {2, 3},
    {1, 2, 3},
    {},
    {1, 6},
    {1, 2},
    {},
    {2, 4},
    {},
    {2, 4},
    {1, 5},
    {1, 2, 5},
    {}
  };

  for(auto&& candidates : candidatesList) {
    for(auto&& c : candidates) {
      c += 48; // upshift from integer to ASCII
    }
  }

  return candidatesList;
}

int main(int, char**)
{
  return (std::ranges::all_of(
    std::views::zip(
      GridCandidates(ReferenceGrid6x6_3x2()),
      ReferenceCandidates6x6_3x2()
    ),
    [](auto t) -> bool {
      auto&& [candidates, reference] = t;
      return std::ranges::equal(candidates, reference);
    }
  )
  ? EXIT_SUCCESS : EXIT_FAILURE);
}
