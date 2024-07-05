#pragma once

#include "grid.h"
#include "grid_check.h"

#include <algorithm>
#include <ranges>

namespace grid_check_detail {

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

inline IsSolved& operator|=(IsSolved& lhs, IsSolved rhs)
{
  if(lhs < rhs) {
    lhs = rhs;
  }
  return lhs;
}

inline bool operator!(IsSolved e)
{
  switch(e) {
    case IsSolved::Yes:
    case IsSolved::NotYet: // keep on looking; we might encounter a Never
      return false;
    default:
      return true;
  }
}

// Sudoku check implemented as struct used as template parameter below so
// it can be replaced in the test of the layered ranges
struct CheckUnique
{
  IsSolved result = IsSolved::Yes;

  inline bool operator()(Grid::container group)
  {
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

} // namespace grid_check_detail

template<typename CheckGroup = grid_check_detail::CheckUnique>
struct Checker
{
  const Grid& grid;
  CheckGroup check{};

  bool CheckFieldRows()
  {
    return std::ranges::all_of(
      std::views::iota(0U, grid.height()),
      [&](size_t row) {
        return check(grid_check_detail::ToGridContainer(
          grid
          | std::views::drop(grid.offsetOf(row, 0))
          | std::views::take(grid.width())
        ));
      }
    );
  }

  bool CheckFieldColumns()
  {
    return std::ranges::all_of(
      std::views::iota(0U, grid.width()),
      [&](size_t col) {
        return check(grid_check_detail::ToGridContainer(
          grid
          | std::views::drop(col)
          | std::views::stride(grid.offsetOf<0>())
        ));
      }
    );
  }

  bool CheckBlocks()
  {
    return std::ranges::all_of( // block rows
      std::views::iota(0U, grid.height() / grid.blockHeight),
      [&](size_t row) {
        return std::ranges::all_of( // block columns
          std::views::iota(0U, grid.width() / grid.blockWidth),
          [&](size_t col) {
            return check(grid_check_detail::ToGridContainer(
              grid // block by row and column
              | std::views::drop(grid.offsetOf(row * grid.blockHeight, col * grid.blockWidth))
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

  operator bool()
  {
    return true
    && CheckFieldRows()
    && CheckFieldColumns()
    && CheckBlocks();
  }
};
