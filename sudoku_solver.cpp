#include "hypervector.h"

#include "lexy/action/parse.hpp"
#include "lexy/callback.hpp"
#include "lexy/dsl.hpp"
#include "lexy/input/string_input.hpp"
#include "lexy_ext/report_error.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string_view>

struct Field
{
  using value_type = unsigned char;

  static constexpr value_type undef = 0;

  value_type num;

  constexpr Field()
    : num(undef)
  {
  }

  constexpr Field(lexy::nullopt)
    : num(undef)
  {
  }

  constexpr Field(value_type num)
    : num(num)
  {
    if(num >= 10) {
      throw std::invalid_argument("out of range");
    }
  }
};

std::ostream& operator<<(std::ostream& os, const Field& f)
{
  os << ' ' << (f.num ? static_cast<char>(f.num + 48) : ' ') << ' ';
  return os;
}

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

// Block is 3x3 Fields
using Grid = hypervector<Field, 2>; // width, height always in multiples of 3

std::ostream& operator<<(std::ostream& os, const Grid& grid)
{
  if(grid.size()) {
    using namespace grid_print_detail;

    auto sepTop = sepTopBlock;
    auto sepLeft = sepLeftBlock;

    for(size_t h = 0; h < grid.size(0); ++h) {
      // print top field border line
      for(size_t w = 0; w < grid.size(0); ++w) {
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

namespace grid_parse_detail {
namespace grammar {

namespace dsl = lexy::dsl;

struct field
{
  // ignore all whitespace encountered within
  static constexpr auto whitespace = dsl::whitespace(dsl::ascii::blank);

  // try to parse an integer value; produce lexy::nullopt on failure
  static constexpr auto rule = dsl::opt(dsl::integer<Field::value_type>);

  // dispatch to the appropriate constructor
  static constexpr auto value = lexy::construct<Field>;
};

struct fields
{
  static constexpr auto rule = [] {
    auto sep = dsl::trailing_sep(dsl::colon | dsl::vbar);
    auto item = dsl::p<field>;

    auto terminator = dsl::terminator(dsl::eol);
    return terminator.list(item, sep);
  }();

  static constexpr auto value = lexy::as_list<std::vector<Field>>;
};

struct production
{
  static constexpr auto rule = [] {
    return dsl::opt(dsl::vbar >> dsl::p<fields>);
  }();

  static constexpr auto value = lexy::as_list<std::vector<Field>>;
};

} // namespace grammar
} // namespace grid_parse_detail

std::istream& operator>>(std::istream& is, Grid& grid)
{
  grid.resize(0, 0);
  grid.reserve(9 * 9); // reasonable size assumption

  std::string str;
  while(std::getline(is, str)) {
    using namespace grid_parse_detail;

    auto input = lexy::string_input(str);
    if(auto parsed = lexy::parse<grammar::production>(input, lexy_ext::report_error); parsed.has_value()) {
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


template<typename T>
T ParseValue(std::string const &arg)
{
  long long num;
  if(!(std::istringstream(arg) >> num) ||
     (num < std::numeric_limits<T>::min()) ||
     (num > std::numeric_limits<T>::max())) {
    throw std::invalid_argument("Invalid number '" + arg + "'");
  }
  return static_cast<T>(num);
}

template<typename T>
T ParseValue(std::string_view arg)
{
  return ParseValue<T>(std::string(arg));
}

int PrintTemplate(unsigned w, unsigned h)
{
  std::cout << Grid(w * 3, h * 3) << std::endl;
  return EXIT_SUCCESS;
}

int PrintTemplate(unsigned w)
{
  return PrintTemplate(w, w);
}

constexpr auto argHelp = std::string_view("--help");
constexpr auto argTemplate = std::string_view("--template");

int Usage(const char* name)
{
  std::cout << "Usage: " << name << "\n"
    << "  [" << argHelp << "]              show this help\n"
    << "  [" << argTemplate << "[=W[xH]]]  print a template of dimensions width x height blocks to use for input\n"
    << std::endl;
  return EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
  if(argc == 2) {
    auto arg = std::string_view(argv[1]);

    if(arg.starts_with(argHelp)) {
      return Usage(argv[0]);
    }

    if(arg.starts_with(argTemplate)) {
      arg.remove_prefix(argTemplate.size());
      if(arg.starts_with('=')) {
        arg.remove_prefix(1);
        if(auto pos = arg.find('x'); pos < arg.size()) {
          return PrintTemplate(
            ParseValue<unsigned>(arg.substr(0, pos)),
            ParseValue<unsigned>(arg.substr(pos + 1, std::string::npos)));
        }
        return PrintTemplate(ParseValue<unsigned>(arg));
      }
      return PrintTemplate(3, 3);
    }
  }

  auto input = R"(
+~~~+~~~+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
|   : 3 :   |   :   :   |   :   :   |
+---+---+---+---+---+---+---+---+---+
|   :   :   | 1 : 9 : 5 |   :   :   |
+---+---+---+---+---+---+---+---+---+
|   :   : 8 |   :   :   |   : 6 :   |
+~~~+~~~+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
| 8 :   :   |   : 6 :   |   :   :   |
+---+---+---+---+---+---+---+---+---+
| 4 :   :   | 8 :   :   |   :   : 1 |
+---+---+---+---+---+---+---+---+---+
|   :   :   |   : 2 :   |   :   :   |
+~~~+~~~+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
|   : 6 :   |   :   :   | 2 : 8 :   |
+---+---+---+---+---+---+---+---+---+
|   :   :   | 4 : 1 : 9 |   :   : 5 |
+---+---+---+---+---+---+---+---+---+
|   :   :   |   :   :   |   : 7 :   |
+~~~+~~~+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
)";
  std::istringstream iss(input);

  Grid grid;
  iss >> grid;

  std::cout << "\nParsed grid:\n\n" << grid << "\n";

  return Usage(argv[0]);
}
