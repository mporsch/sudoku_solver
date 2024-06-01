#pragma once

#include "grid.h"

#include <iostream>
#include <string_view>

namespace grid_print_detail {

struct SepField
{
  std::string_view str;

  constexpr SepField(const char* str)
    : str(str)
  {
  }
};

std::ostream& operator<<(std::ostream& os, const SepField& sep)
{
  os << sep.str;
  return os;
}

constexpr auto sepTopBlock = SepField("+~~~");
constexpr auto sepTopField = SepField("+---");
constexpr auto sepLeftBlock = SepField("|");
constexpr auto sepLeftField = SepField(":");
constexpr auto sepRightBlockBorder = SepField("+\n");
constexpr auto sepRightBlockField = SepField("|\n");

} // namespace grid_print_detail

std::ostream& operator<<(std::ostream& os, const Field& f)
{
  os << ' ' << (f.num ? static_cast<char>(f.num + 48) : ' ') << ' ';
  return os;
}

std::ostream& operator<<(std::ostream& os, const Grid& grid)
{
  if(grid.size()) {
    using namespace grid_print_detail;

    auto sepTop = sepTopBlock;
    auto sepLeft = sepLeftBlock;

    for(size_t h = 0; h < grid.size(0); ++h) {
      // print top field border line
      for(size_t w = 0; w < grid.size(1); ++w) {
        os << sepTop;
      }
      os << sepRightBlockBorder;

      // print field line
      for(size_t w = 0; w < grid.size(1); ++w) {
        os << sepLeft << grid.at(h, w);
        sepLeft = ((w + 1) % 3 ? sepLeftField : sepLeftBlock);
      }
      os << sepRightBlockField;

      sepTop = ((h + 1) % 3 ? sepTopField : sepTopBlock);
    }

    // print bottom block border line
    for(size_t w = 0; w < grid.size(1); ++w) {
      os << sepTopBlock;
    }
    os << sepRightBlockBorder;
  }
  return os;
}
