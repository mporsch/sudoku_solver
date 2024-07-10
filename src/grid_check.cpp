#include "grid_check.h"
#include "grid_algo.h"

#include <algorithm>
#include <ranges>

namespace {

Grid::container ToGridContainer(std::ranges::viewable_range auto&& r)
{
  if constexpr(std::ranges::forward_range<decltype(r)>) {
    return Grid::container(r.begin(), r.end());
  }

  Grid::container c;
  c.reserve(9); // reasonable size assumption
  for(auto&&f : r) {
    c.push_back(f);
  }
  return c;
}

IsSolved& operator|=(IsSolved& lhs, IsSolved rhs)
{
  if(lhs < rhs) {
    lhs = rhs;
  }
  return lhs;
}

bool operator!(IsSolved e)
{
  switch(e) {
    case IsSolved::Yes:
    case IsSolved::NotYet: // keep on looking; we might encounter a Never
      return false;
    default:
      return true;
  }
}

// Sudoku check implemented as struct to be used as predicate to all_of
struct CheckUnique
{
  IsSolved result = IsSolved::Yes;

  bool operator()(std::ranges::viewable_range auto&& r)
  {
    auto group = ToGridContainer(r);

    std::sort(begin(group), end(group));

    // find where the empty fields end and the field values begin
    auto mid = std::find_if(begin(group), end(group), [](const Field& f) { return f != Field(); });

    if(std::unique(mid, end(group)) != end(group)) {
      result |= IsSolved::Never;
    } else if(mid != begin(group)) {
      result |= IsSolved::NotYet;
    }
    return !!result;
  }
};

} // namespace anonymous

IsSolved Check(const Grid& grid)
{
  auto checker = CheckUnique{};
  (void)allOf(grid, checker); // this is a concession for using all_of internally
  return checker.result;
}
