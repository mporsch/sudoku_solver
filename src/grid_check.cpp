#include "grid_check.h"
#include "grid_algo.h"

#include <algorithm>
#include <ranges>

namespace {

// Sudoku check implemented to be used as predicate to all_of
struct CheckUnique
{
  bool operator()(std::ranges::viewable_range auto range) const
  {
    auto group = To<Digits>(range);
    std::sort(begin(group), end(group));

    // find where the unsolved fields end and the field values begin
    auto mid = std::find_if(
      begin(group), end(group),
      [](auto d) { return (d != Field::undef); }
    );

    return (std::unique(mid, end(group)) == end(group));
  }
};

} // namespace anonymous

bool IsSolvable(const Grid& grid)
{
  return AllGroupsOf(grid, CheckUnique{});
}
