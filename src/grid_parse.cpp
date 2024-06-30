#include "grid_parse.h"

#include "lexy/action/parse.hpp"
#include "lexy/callback.hpp"
#include "lexy/dsl.hpp"
#include "lexy/input/string_input.hpp"
#include "lexy_ext/report_error.hpp"

#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

namespace dsl = lexy::dsl;

Field MakeField(lexy::nullopt)
{
  return Field{Field::undef};
}

Field MakeField(lexy::lexeme<lexy::_prd> num)
{
  return Field{static_cast<Field::value_type>(*num.begin() - 48)}; // downshift from ASCII to integer
}

struct production
{
  struct field
  {
    // try to parse a character value; produce lexy::nullopt on failure
    static constexpr auto rule = dsl::opt(dsl::identifier(dsl::ascii::alnum));

    // dispatch to the appropriate constructor
    static constexpr auto value = lexy::callback<Field>([](auto&&... args) { return MakeField(args...); });
  };

  struct fields
  {
    // fields separated by :
    static constexpr auto rule = [] {
      auto item = dsl::p<field>;
      auto sep = dsl::sep(dsl::colon);
      return dsl::list(item, sep);
    }();

    // create list of fields
    static constexpr auto value = lexy::as_list<std::vector<Field>>;
  };

  struct blocks
  {
    // whole line with blocks separated by |
    static constexpr auto rule = [] {
      auto terminator = dsl::terminator(dsl::eol);
      auto item = dsl::p<fields>;
      auto sep = dsl::trailing_sep(dsl::vbar);
      return terminator.list(item, sep);
    }();

    // create list of blocks
    static constexpr auto value = lexy::as_list<std::vector<std::vector<Field>>>;
  };

  // ignore all whitespace encountered here and in sub-rules
  static constexpr auto whitespace = dsl::whitespace(dsl::ascii::blank);

  // non-border block/field lines start with a |
  static constexpr auto rule = dsl::opt(dsl::vbar >> dsl::p<blocks>);

  // forward list of fields created by sub-rule or create empty list
  static constexpr auto value = lexy::as_list<std::vector<std::vector<Field>>>;
};

bool parse_field_line(Grid& grid, const std::string& str)
{
  auto input = lexy::string_input(str);
  if(auto parsed = lexy::parse<production>(input, lexy_ext::report_error); parsed.has_value()) {
    if(auto blocks = parsed.value(); !blocks.empty()) {
      auto fields_width = std::accumulate(
        begin(blocks), end(blocks),
        0U,
        [](size_t sum, auto&& block) {
          return sum + block.size();
        });

      if(auto current_width = grid.width()) {
        if(current_width != fields_width) {
          throw std::invalid_argument("unequal width of grid fields");
        }
      }

      auto block_width = blocks.front().size();
      if(grid.blockWidth) {
        if(grid.blockWidth != block_width) {
          throw std::invalid_argument("unequal width of grid blocks");
        }
      } else {
        grid.blockWidth = block_width;
      }

      auto current_height = grid.height();
      grid.resize(current_height + 1, fields_width);
      auto current_row = grid[current_height];
      auto pos = current_row.begin();
      for(auto&& block : blocks) {
        pos = std::copy(begin(block), end(block), pos);
      }

      return true;
    }
  }
  return false;
}

bool parse_block_height(Grid& grid, const std::string& str)
{
  if(auto grid_height = grid.height()) {
    if(str.starts_with("+~")) { // block border line
      if(grid.blockHeight) {
        if(grid_height % grid.blockHeight != 0) {
          throw std::invalid_argument("unequal height of grid blocks");
        }
      } else {
        grid.blockHeight = grid_height;
      }
    }
  }
  return true;
}

} // namespace anonymous

std::istream& operator>>(std::istream& is, Grid& grid)
{
  grid.resize(0, 0);
  grid.reserve(9 * 9); // reasonable size assumption

  std::string str;
  while(std::getline(is, str)) {
    parse_field_line(grid, str) || parse_block_height(grid, str);
  }
  return is;
}
