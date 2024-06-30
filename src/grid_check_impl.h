#pragma once

#include "grid.h"

#include <algorithm>
#include <ranges>

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

// Sudoku check implemented as struct used as template parameter below so
// it can be replaced in the test of the layered ranges
struct CheckUnique
{
  bool operator()(Grid::container group) const
  {
    std::sort(begin(group), end(group));

    if(group.front().num == Field::undef) {
      return false;
    }
    if(std::unique(begin(group), end(group)) != end(group)) {
      return false;
    }
    return true;
  }
};

template<typename CheckGroup = CheckUnique>
struct Checker
{
  const Grid& grid;

  operator bool() const
  {
    return true
    && std::ranges::all_of( // field rows
      std::views::iota(0U, grid.height()),
      [&](size_t row) {
        return CheckGroup()(ToGridContainer(
          grid
          | std::views::drop(row * grid.offsetOf<0>())
          | std::views::take(grid.width())
        ));
      }
    )
    && std::ranges::all_of( // field columns
      std::views::iota(0U, grid.width()),
      [&](size_t col) {
        return CheckGroup()(ToGridContainer(
          grid
          | std::views::drop(col)
          | std::views::stride(grid.offsetOf<0>())
        ));
      }
    )
    && std::ranges::all_of( // block rows
      std::views::iota(0U, grid.height() / grid.blockHeight),
      [&](size_t row) {
        return std::ranges::all_of( // block columns
          std::views::iota(0U, grid.width() / grid.blockWidth),
          [&](size_t col) {
            return CheckGroup()(ToGridContainer(
              grid // blocks by row and column
              | std::views::drop(row * grid.offsetOf<0>() * grid.blockHeight + col * grid.blockWidth)
              | std::views::slide(grid.blockWidth)
              | std::views::stride(grid.offsetOf<0>())
              | std::views::take(grid.blockHeight)
              | std::views::join
            ));
          }
        );
      }
    );
  }
};
