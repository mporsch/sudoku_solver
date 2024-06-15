#include "grid_check.h"
#include "custom_step_iterator.h"
#include "dance_dance_iterator.h"
#include "preserve_iterator.h"

#include <algorithm>
#include <utility>

namespace {

auto FieldRowRange(Grid::const_iterator it, const Grid& grid)
{
  auto first = make_custom_step_iterator(it, grid.offsetOf<0>());
  auto last = std::next(first, grid.height());
  return std::make_pair(
    make_preserve_iterator(first),
    make_preserve_iterator(last));
}

auto FieldRowColRange(Grid::const_iterator it, const Grid& grid)
{
  return std::make_pair(it, std::next(it, grid.width()));
}

auto FieldColRange(Grid::const_iterator it, const Grid& grid)
{
  return std::make_pair(
    make_preserve_iterator(it),
    make_preserve_iterator(std::next(it, grid.width())));
}

auto FieldColRowRange(Grid::const_iterator it, const Grid& grid)
{
  auto first = make_custom_step_iterator(it, grid.offsetOf<0>());
  auto last = std::next(first, grid.height());
  return std::make_pair(first, last);
}

auto BlockRowRange(Grid::const_iterator it, const Grid& grid)
{
  auto first = make_custom_step_iterator(it,
    grid.offsetOf<0>() * Grid::BlockSize);
  auto last = std::next(first, grid.height() / Grid::BlockSize);
  return std::make_pair(
    make_preserve_iterator(first),
    make_preserve_iterator(last));
}

auto BlockRowColRange(Grid::const_iterator it, const Grid& grid)
{
  auto first = make_custom_step_iterator(it, Grid::BlockSize);
  auto last = std::next(first, grid.width() / Grid::BlockSize);
  return std::make_pair(
    make_preserve_iterator(first),
    make_preserve_iterator(last));
}

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

bool CheckUnique(Grid::container sorted)
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

struct Checker
{
  const Grid& grid;

  template<typename FwdId>
  static bool CheckRange(std::pair<FwdId, FwdId> range)
  {
    return CheckUnique(Grid::container(range.first, range.second));
  }

  template<typename FwdId, typename... Fs>
  bool CheckRange(
    std::pair<FwdId, FwdId> range,
    Fs&&... getNextRanges) const
  {
    return std::all_of(range.first, range.second,
      [&](auto it) {
        return Check(it, std::forward<Fs>(getNextRanges)...);
      });
  }

  template<typename FwdId, typename F, typename... Fs>
  bool Check(
    FwdId it,
    F&& getRange,
    Fs&&... getNextRanges) const
  {
    return CheckRange(
      getRange(it, grid),
      std::forward<Fs>(getNextRanges)...);
  }

  operator bool() const
  {
    return true
    && Check(grid.begin(), FieldRowRange, FieldRowColRange)
    && Check(grid.begin(), FieldColRange, FieldColRowRange)
    && Check(grid.begin(), BlockRowRange, BlockRowColRange, BlockFieldsRange);
  }
};

} // namespace anonymous

bool IsSolved(const Grid& grid)
{
  return Checker{grid};
}
