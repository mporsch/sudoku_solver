#pragma once

#include "grid.h"

#include <algorithm>
#include <ranges>

namespace grid_algo_detail {

std::ranges::viewable_range auto FieldRowRanges(auto&& grid)
{
  return grid | std::views::chunk(grid.width());
}

std::ranges::viewable_range auto FieldColumnRanges(auto&& grid)
{
  // sliding window of this shape
  // | x | x | x |
  // | x | x | x |
  // | x |   |   |

  return grid
  | std::views::slide((grid.height() - 1) * grid.template offsetOf<0>() + 1)
  | std::views::transform([&](auto&& range) {
    // apply stride within window (not to range of windows)

    return range | std::views::stride(grid.template offsetOf<0>());
  });
}

std::ranges::viewable_range auto BlockRanges(auto&& grid)
{
  return grid
  | std::views::chunk(grid.blockHeight * grid.width())
  | std::views::transform([&](auto&& blockRowRange) {
    // sliding window shaped as above, but over blocks instead of fields

    return blockRowRange
    | std::views::slide((grid.blockHeight - 1) * grid.template offsetOf<0>() + grid.blockWidth)
    | std::views::stride(grid.blockWidth)
    | std::views::transform([&](auto&& range) {
      // block fields by row and column

      return range
      | std::views::chunk(grid.blockWidth)
      | std::views::stride(grid.template offsetOf<0>() / grid.blockWidth)
      | std::views::join;
    });
  })
  | std::views::join;
}

} // namespace grid_algo_detail

template<typename Pred>
bool AllGroupsOf(const Grid& grid, Pred&& pred)
{
  using namespace grid_algo_detail;

  return true
  && std::ranges::all_of(FieldRowRanges(grid), pred)
  && std::ranges::all_of(FieldColumnRanges(grid), pred)
  && std::ranges::all_of(BlockRanges(grid), pred);
}

template<typename UnaryFunc>
void ForEachGroup(Grid& grid, UnaryFunc&& func)
{
  using namespace grid_algo_detail;

  std::ranges::for_each(FieldRowRanges(grid), func);
  std::ranges::for_each(FieldColumnRanges(grid), func);
  std::ranges::for_each(BlockRanges(grid), func);
}

// should work for Digits or Fields
template<typename Container>
Container To(std::ranges::viewable_range auto&& range)
{
  if constexpr(std::ranges::forward_range<decltype(range)>) {
    return Container(range.begin(), range.end());
  }

  Container c;
  c.reserve(9); // reasonable size assumption
  for(auto&& f : range) {
    c.push_back(f);
  }
  return c;
}
