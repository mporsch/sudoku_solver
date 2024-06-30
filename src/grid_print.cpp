#include "grid_print.h"

#include <string_view>

namespace {

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

} // namespace anonymous

std::ostream& operator<<(std::ostream& os, const Field& f)
{
  os << ' ' << (f.num ? static_cast<char>(f.num + 48) : ' ') << ' ';
  return os;
}

std::ostream& operator<<(std::ostream& os, const Grid& grid)
{
  if(grid.size()) {
    auto sepTop = sepTopBlock;
    auto sepLeft = sepLeftBlock;

    for(size_t h = 0; h < grid.height(); ++h) {
      // print top field border line
      for(size_t w = 0; w < grid.width(); ++w) {
        os << sepTop;
      }
      os << sepRightBlockBorder;

      // print field line
      for(size_t w = 0; w < grid.width(); ++w) {
        os << sepLeft << grid.at(h, w);
        sepLeft = ((w + 1) % grid.blockWidth ? sepLeftField : sepLeftBlock);
      }
      os << sepRightBlockField;

      sepTop = ((h + 1) % grid.blockHeight ? sepTopField : sepTopBlock);
    }

    // print bottom block border line
    for(size_t w = 0; w < grid.width(); ++w) {
      os << sepTopBlock;
    }
    os << sepRightBlockBorder;
  }
  return os;
}
