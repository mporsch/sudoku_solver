#pragma once

#include "grid.h"

#include <algorithm>
#include <ranges>

namespace grid_algo_detail {

std::ranges::viewable_range auto FieldRowRange(auto&& grid, size_t row)
{
  return grid
  | std::views::drop(grid.offsetOf(row, 0))
  | std::views::take(grid.width());
}

std::ranges::viewable_range auto FieldColumnRange(auto&& grid, size_t col)
{
  return grid
  | std::views::drop(col)
  | std::views::stride(grid.template offsetOf<0>());
}

std::ranges::viewable_range auto BlockRange(auto&& grid, size_t row, size_t col)
{
  return grid // block by row and column
  | std::views::drop(grid.offsetOf(row * grid.blockHeight, col * grid.blockWidth))
  | std::views::slide(grid.blockWidth)
  | std::views::stride(grid.template offsetOf<0>())
  | std::views::take(grid.blockHeight)
  | std::views::join;
}

struct AllGroupsOf
{
  template<typename Pred>
  static bool CheckFieldRows(const Grid& grid, Pred&& pred)
  {
    return std::ranges::all_of(
      std::views::iota(0U, grid.height()),
      [&](size_t row) { return pred(FieldRowRange(grid, row)); }
    );
  }

  template<typename Pred>
  static bool CheckFieldColumns(const Grid& grid, Pred&& pred)
  {
    return std::ranges::all_of(
      std::views::iota(0U, grid.width()),
      [&](size_t col) { return pred(FieldColumnRange(grid, col)); }
    );
  }

  template<typename Pred>
  static bool CheckBlocks(const Grid& grid, Pred&& pred)
  {
    return std::ranges::all_of( // block rows
      std::views::iota(0U, grid.height() / grid.blockHeight),
      [&](size_t row) {
        return std::ranges::all_of( // block columns
          std::views::iota(0U, grid.width() / grid.blockWidth),
          [&](size_t col) { return pred(BlockRange(grid, row, col)); }
        );
      }
    );
  }

  template<typename Pred>
  bool operator()(const Grid& grid, Pred&& pred) const
  {
    return true
    && CheckFieldRows(grid, pred)
    && CheckFieldColumns(grid, pred)
    && CheckBlocks(grid, pred);
  }
};

struct ForEachGroup
{
  template<typename UnaryFunc>
  static void ForFieldRows(Grid& grid, UnaryFunc&& func)
  {
    std::ranges::for_each(
      std::views::iota(0U, grid.height()),
      [&](size_t row) { func(FieldRowRange(grid, row)); }
    );
  }

  template<typename UnaryFunc>
  static void ForFieldColumns(Grid& grid, UnaryFunc&& func)
  {
    std::ranges::for_each(
      std::views::iota(0U, grid.width()),
      [&](size_t col) { func(FieldColumnRange(grid, col)); }
    );
  }

  template<typename UnaryFunc>
  static void ForBlocks(Grid& grid, UnaryFunc&& func)
  {
    std::ranges::for_each( // block rows
      std::views::iota(0U, grid.height() / grid.blockHeight),
      [&](size_t row) {
        std::ranges::for_each( // block columns
          std::views::iota(0U, grid.width() / grid.blockWidth),
          [&](size_t col) { func(BlockRange(grid, row, col)); }
        );
      }
    );
  }

  template<typename UnaryFunc>
  void operator()(Grid& grid, UnaryFunc&& func) const
  {
    ForFieldRows(grid, func);
    ForFieldColumns(grid, func);
    ForBlocks(grid, func);
  }
};

} // namespace grid_algo_detail

constexpr grid_algo_detail::AllGroupsOf AllGroupsOf;
constexpr grid_algo_detail::ForEachGroup ForEachGroup;

Grid::container ToFields(std::ranges::viewable_range auto&& range)
{
  if constexpr(std::ranges::forward_range<decltype(range)>) {
    return Grid::container(range.begin(), range.end());
  }

  Grid::container c;
  c.reserve(9); // reasonable size assumption
  for(auto&&f : range) {
    c.push_back(f);
  }
  return c;
}
