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
  c.reserve(Grid::BlockSize * Grid::BlockSize);
  for(auto&&f : r) {
    c.push_back(f);
  }
  return c;
}

// Sudoku check implemented as struct used as template parameter below so
// it can be replaced in the test of the layered ranges
struct CheckUnique
{
  bool operator()(Grid::container sorted) const
  {
    std::sort(begin(sorted), end(sorted));

    if(sorted.front().num == Field::undef) {
      return false;
    }
    if(std::unique(begin(sorted), end(sorted)) != end(sorted)) {
      return false;
    }
    return true;
  }
};

template<typename CheckFields = CheckUnique>
struct Checker
{
  const Grid& grid;

  operator bool() const
  {
    return true
    && std::ranges::all_of( // field rows
      std::views::iota(0U, grid.height()),
      [&](size_t row) {
        return CheckFields()(ToGridContainer(
          grid
          | std::views::drop(row * grid.offsetOf<0>())
          | std::views::take(grid.width())
        ));
      }
    )
    && std::ranges::all_of( // field columns
      std::views::iota(0U, grid.width()),
      [&](size_t col) {
        return CheckFields()(ToGridContainer(
          grid
          | std::views::drop(col)
          | std::views::stride(grid.offsetOf<0>())
        ));
      }
    )
    && std::ranges::all_of( // block rows
      std::views::iota(0U, grid.height() / Grid::BlockSize),
      [&](size_t row) {
        return std::ranges::all_of( // block columns
          std::views::iota(0U, grid.width() / Grid::BlockSize),
          [&](size_t col) {
            return CheckFields()(ToGridContainer(
              grid // blocks by row and column
              | std::views::drop((row * grid.offsetOf<0>() + col) * Grid::BlockSize)
              | std::views::slide(Grid::BlockSize)
              | std::views::stride(grid.offsetOf<0>())
              | std::views::take(Grid::BlockSize)
              | std::views::join
            ));
          }
        );
      }
    );
  }
};
