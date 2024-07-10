#pragma once

#include "grid.h"

#include <algorithm>
#include <ranges>

struct AllOf
{
  template<typename Pred>
  static bool CheckFieldRows(const Grid& grid, Pred&& pred)
  {
    return std::ranges::all_of(
      std::views::iota(0U, grid.height()),
      [&](size_t row) {
        return pred(
          grid
          | std::views::drop(grid.offsetOf(row, 0))
          | std::views::take(grid.width())
        );
      }
    );
  }

  template<typename Pred>
  static bool CheckFieldColumns(const Grid& grid, Pred&& pred)
  {
    return std::ranges::all_of(
      std::views::iota(0U, grid.width()),
      [&](size_t col) {
        return pred(
          grid
          | std::views::drop(col)
          | std::views::stride(grid.offsetOf<0>())
        );
      }
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
          [&](size_t col) {
            return pred(
              grid // block by row and column
              | std::views::drop(grid.offsetOf(row * grid.blockHeight, col * grid.blockWidth))
              | std::views::slide(grid.blockWidth)
              | std::views::stride(grid.offsetOf<0>())
              | std::views::take(grid.blockHeight)
              | std::views::join
            );
          }
        );
      }
    );
  }

  template<typename Pred>
  bool operator()(const Grid& grid, Pred&& pred)
  {
    return true
    && CheckFieldRows(grid, pred)
    && CheckFieldColumns(grid, pred)
    && CheckBlocks(grid, pred);
  }
};

static AllOf allOf;
