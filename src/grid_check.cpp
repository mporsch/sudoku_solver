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
  { // check rows
    auto firstFieldRow = make_custom_step_iterator(grid.begin(), grid.offsetOf<0>());
    auto lastFieldRow = std::next(firstFieldRow, grid.height());
    for(; firstFieldRow != lastFieldRow; ++firstFieldRow) {
      auto firstField = firstFieldRow.base();
      auto lastField = std::next(firstField, grid.width());
      if(!IsSolved(Grid::container(firstField, lastField))) {
        return false;
      }
    }
  }

  { // check columns
    auto firstFieldCol = grid.begin();
    auto lastFieldCol = std::next(firstFieldCol, grid.offsetOf<0>());
    for(; firstFieldCol != lastFieldCol; ++firstFieldCol) {
      auto firstField = make_custom_step_iterator(firstFieldCol, grid.offsetOf<0>());
      auto lastField = std::next(firstField, 1);
      if(!IsSolved(Grid::container(firstField, lastField))) {
        return false;
      }
    }
  }

  { // check blocks
    auto firstBlockRow = make_custom_step_iterator(grid.begin(), grid.offsetOf<0>() * 3);
    auto lastBlockRow = std::next(firstBlockRow, grid.height() / 3);
    for(; firstBlockRow != lastBlockRow; ++firstBlockRow) {
      auto firstBlockCol = make_custom_step_iterator(firstBlockRow.base(), 3);
      auto lastBlockCol = std::next(firstBlockCol, grid.offsetOf<0>() / 3);
      for(; firstBlockCol != lastBlockCol; ++firstBlockCol) {
        auto firstField = make_dance_dance_iterator(firstBlockCol.base(),
           1,  1, grid.offsetOf<0>(),
          -1, -1, grid.offsetOf<0>(),
           1,  1, -2 - 2 * grid.offsetOf<0>()); // the last step goes back to the start
        auto lastField = make_dance_dance_iterator(firstField.base());
        if(!IsSolved(Grid::container(firstField, lastField))) {
          return false;
        }
      }
    }
  }

  return true;
}
