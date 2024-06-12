#include "grid_check.h"
#include "custom_step_iterator.h"
#include "dance_dance_iterator.h"
#include "preserve_iterator.h"

#include <algorithm>
#include <utility>

namespace {

template<typename FwdId>
struct Range : std::pair<FwdId, FwdId>
{
  FwdId begin() const noexcept
  {
    return this->first;
  }

  FwdId end() const noexcept
  {
    return this->second;
  }
};

template<typename FwdId>
Range<FwdId> MakeRange(std::pair<FwdId, FwdId> range)
{
  return Range<FwdId>{std::move(range)};
}

auto FieldRowRange(const Grid& grid)
{
  auto first = make_custom_step_iterator(grid.begin(), grid.offsetOf<0>());
  auto last = std::next(first, grid.height());
  return MakeRange(std::make_pair(
    make_preserve_iterator(first),
    make_preserve_iterator(last)));
}

auto FieldRowColRange(Grid::const_iterator it, const Grid& grid)
{
  return std::make_pair(it, std::next(it, grid.width()));
}

auto FieldColRange(const Grid& grid)
{
  auto first = grid.begin();
  auto last = std::next(first, grid.offsetOf<0>());
  return MakeRange(std::make_pair(
    make_preserve_iterator(first),
    make_preserve_iterator(last)));
}

auto FieldColRowRange(Grid::const_iterator it, const Grid& grid)
{
  auto first = make_custom_step_iterator(it, grid.offsetOf<0>());
  auto last = std::next(first, 1);
  return std::make_pair(first, last);
}

auto BlockRowRange(const Grid& grid)
{
  auto first = make_custom_step_iterator(grid.begin(),
    grid.offsetOf<0>() * Grid::BlockSize);
  auto last = std::next(first, grid.height() / Grid::BlockSize);
  return MakeRange(std::make_pair(
    make_preserve_iterator(first),
    make_preserve_iterator(last)));
}

auto BlockRowColRange(Grid::const_iterator it, const Grid& grid)
{
  auto first = make_custom_step_iterator(it, Grid::BlockSize);
  auto last = std::next(first, grid.offsetOf<0>() / Grid::BlockSize);
  return MakeRange(std::make_pair(
    make_preserve_iterator(first),
    make_preserve_iterator(last)));
}

auto BlockRange(Grid::const_iterator it, const Grid& grid)
{
  auto first = make_dance_dance_iterator(it,
     1,  1, grid.offsetOf<0>(),
    -1, -1, grid.offsetOf<0>(),
     1,  1, -2 - 2 * grid.offsetOf<0>()); // the last step goes back to the start
  auto last = make_dance_dance_iterator(first.base());
  return std::make_pair(first, last);
}

bool IsSolved(Grid::container sorted)
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

template<typename FwdId>
bool IsSolved(std::pair<FwdId, FwdId> range)
{
  return IsSolved(Grid::container(range.first, range.second));
}

} // namespace anonymous

bool IsSolved(const Grid& grid)
{
  // check rows
  for(auto row : FieldRowRange(grid)) {
    if(!IsSolved(FieldRowColRange(row.base(), grid))) {
      return false;
    }
  }

  // check columns
  for(auto col : FieldColRange(grid)) {
    if(!IsSolved(FieldColRowRange(col, grid))) {
      return false;
    }
  }

  // check blocks
  for(auto row : BlockRowRange(grid)) {
    for(auto col : BlockRowColRange(row, grid)) {
      if(!IsSolved(BlockRange(col, grid))) {
        return false;
      }
    }
  }

  return true;
}
