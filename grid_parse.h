#pragma once

#include "grid.h"

#include "lexy/action/parse.hpp"
#include "lexy/callback.hpp"
#include "lexy/dsl.hpp"
#include "lexy/input/string_input.hpp"
#include "lexy_ext/report_error.hpp"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace grid_parse_detail {

namespace dsl = lexy::dsl;

struct production
{
  struct field
  {
    // try to parse an integer value; produce lexy::nullopt on failure
    static constexpr auto rule = dsl::opt(dsl::integer<Field::value_type>);

    // dispatch to the appropriate constructor
    static constexpr auto value = lexy::construct<Field>;
  };

  struct fields
  {
    // whole line with fields separated by either : or |
    static constexpr auto rule = [] {
      auto terminator = dsl::terminator(dsl::eol);
      auto item = dsl::p<field>;
      auto sep = dsl::trailing_sep(dsl::colon | dsl::vbar);
      return terminator.list(item, sep);
    }();

    // create list of fields
    static constexpr auto value = lexy::as_list<std::vector<Field>>;
  };

  // ignore all whitespace encountered here and in sub-rules
  static constexpr auto whitespace = dsl::whitespace(dsl::ascii::blank);

  // non-border field lines start with a |
  static constexpr auto rule = dsl::opt(dsl::vbar >> dsl::p<fields>);

  // forward list of fields created by sub-rule or create empty list
  static constexpr auto value = lexy::as_list<std::vector<Field>>;
};

} // namespace grid_parse_detail

std::istream& operator>>(std::istream& is, Grid& grid)
{
  grid.resize(0, 0);
  grid.reserve(9 * 9); // reasonable size assumption

  std::string str;
  while(std::getline(is, str)) {
    using namespace grid_parse_detail;

    auto input = lexy::string_input(str);
    if(auto parsed = lexy::parse<production>(input, lexy_ext::report_error); parsed.has_value()) {
      if(auto fields = parsed.value(); !fields.empty()) {
        if(auto current_width = grid.size(1)) {
          if(current_width != fields.size()) {
            throw std::invalid_argument("unequal width of grid lines");
          }
        }

        auto current_height = grid.size(0);
        grid.resize(current_height + 1, fields.size());
        auto current_row = grid[current_height];
        (void)std::copy(begin(fields), end(fields), current_row.begin());
      }
    }
  }
  return is;
}
