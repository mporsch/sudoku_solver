#include "hypervector.h"

#include <cstdlib>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string_view>

struct Field
{
  unsigned char num;

  constexpr Field(unsigned char num)
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

constexpr auto undef = Field(0);


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
  using namespace grid_print_detail;

  auto sepTop = sepTopBlock;
  auto sepLeft = sepLeftBlock;

  for(size_t h = 0; h < grid.size(1); ++h) {
    // print top field border line
    for(size_t w = 0; w < grid.size(0); ++w) {
      os << sepTop;
    }
    os << sepRightBlockBorder;

    // print field line
    for(size_t w = 0; w < grid.size(0); ++w) {
      os << sepLeft << grid.at(w, h);
      sepLeft = ((w + 1) % 3 ? sepLeftField : sepLeftBlock);
    }
    os << sepRightBlockField;

    sepTop = ((h + 1) % 3 ? sepTopField : sepTopBlock);
  }

  // print bottom block border line
  for(size_t w = 0; w < grid.size(0); ++w) {
    os << sepTopBlock;
  }
  os << sepRightBlockBorder;

  return os;
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
  std::cout << Grid(w * 3, h * 3, undef) << std::endl;
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

  return Usage(argv[0]);
}
