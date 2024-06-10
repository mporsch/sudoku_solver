#include "grid_check.h"
#include "custom_step_iterator.h"
#include "dance_dance_iterator.h"

#include <algorithm>

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

bool IsSolved(const Grid& grid)
{
  // check rows
  for(size_t h = 0; h < grid.height(); ++h) {
    auto first = &grid.at(h, 0);
    auto last = &grid.at(h, grid.width());

    if(!IsSolved(Grid::container(first, last))) {
      return false;
    }
  }

  // check columns
  for(size_t w = 0; w < grid.width(); ++w) {
    auto first = make_custom_step_iterator(
      std::next(grid.begin(), w),
      grid.offsetOf<0>());
    auto last = std::next(first, 1);

    if(!IsSolved(Grid::container(first, last))) {
      return false;
    }
  }

  { // check blocks
    auto it = grid.begin();
    for(size_t h = 0; h < grid.height(); h += 3) {
      for(size_t w = 0; w < grid.width(); w += 3) {
        auto first = make_dance_dance_iterator(it,
           1,  1, grid.offsetOf<0>(),
          -1, -1, grid.offsetOf<0>(),
           1,  1, -2 - 2 * grid.offsetOf<0>()); // the last step goes back to the start
        auto last = make_dance_dance_iterator(it);

        if(!IsSolved(Grid::container(first, last))) {
          return false;
        }

        it += 3;
      }

      it += 2 * grid.offsetOf<0>();
    }
  }

  return true;
}
