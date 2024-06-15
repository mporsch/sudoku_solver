#pragma once

#include "custom_step_iterator.h"
#include "dance_dance_iterator.h"
#include "grid.h"
#include "preserve_iterator.h"

#include <algorithm>
#include <utility>

/// Ensure that dereferencing an iterator of the range returns
/// the iterator itself (instead of a field reference) which can be used to
/// create the next range
template<typename FwdId>
auto PreserveRange(std::pair<FwdId, FwdId> range)
{
  return std::make_pair(
    make_preserve_iterator(range.first),
    make_preserve_iterator(range.second));
}

/// Iterate over grid vertically (over whole rows or fields in a column)
/// @tparam  Step  Size to distinguish between stepping over fields or blocks
/// @param  it  Starting point; must be in first row of \p grid
template<size_t Step>
auto RowRange(Grid::const_iterator it, const Grid& grid)
{
  auto first = make_custom_step_iterator(it, grid.offsetOf<0>() * Step);
  auto last = std::next(first, grid.height() / Step);
  return std::make_pair(first, last);
}

/// Iterate over grid horizontally (over whole columns or fields in a row)
/// @tparam  Step  Size to distinguish between stepping over fields or blocks
/// @param  it  Starting point; must be in first column of \p grid
template<size_t Step>
auto ColRange(Grid::const_iterator it, const Grid& grid)
{
  auto first = make_custom_step_iterator(it, Step);
  auto last = std::next(first, grid.width() / Step);
  return std::make_pair(first, last);
}

constexpr auto FieldRowRange = RowRange<1>;
constexpr auto FieldColRange = ColRange<1>;
constexpr auto BlockRowRange = RowRange<Grid::BlockSize>;
constexpr auto BlockColRange = ColRange<Grid::BlockSize>;

auto BlockFieldsRange(Grid::const_iterator it, const Grid& grid)
{
  // start at the top left field of a block
  auto first = make_dance_dance_iterator(it, // +~~~~~~~+
     1,  1, grid.offsetOf<0>(),              // | > > v |
    -1, -1, grid.offsetOf<0>(),              // | v < < |
     1,  1, -2 - 2 * grid.offsetOf<0>());    // | > > O |
  auto last = make_dance_dance_iterator(it); // +~~~~~~~+
  return std::make_pair(first, last); // the last step goes back to the start
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

  /// Variadic template to concatenate all_of iterations of layered ranges
  template<typename FwdId, typename F, typename... Fs>
  bool Check(
    FwdId it,
    F&& getRange,
    Fs&&... getNextRanges) const
  {
    auto range = PreserveRange(getRange(it, grid));
    return std::all_of(range.first, range.second,
      [&](auto it) {
        return Check(it, std::forward<Fs>(getNextRanges)...);
      });
  }

  /// Variadic template terminator to create the final range which
  /// determines a list of fields to call the actual Sudoku check with
  template<typename FwdId, typename F>
  bool Check(
    FwdId it,
    F&& getRange) const
  {
    auto range = getRange(it, grid);
    return CheckFields()(Grid::container(range.first, range.second));
  }

  operator bool() const
  {
    return true
    && Check(grid.begin(), FieldRowRange, FieldColRange) // field rows
    && Check(grid.begin(), FieldColRange, FieldRowRange) // field columns
    && Check(grid.begin(), BlockRowRange, BlockColRange, BlockFieldsRange); // blocks by row and column
  }
};
