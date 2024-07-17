#include "args_parse.h"

#include "lexy/action/parse.hpp"
#include "lexy/callback.hpp"
#include "lexy/dsl.hpp"
#include "lexy/input/argv_input.hpp"
#include "lexy_ext/report_error.hpp"

#include <cmath>
#include <stdexcept>

#define ARG_HELP "--help"
#define ARG_TEMPLATE "--template"

namespace {

namespace dsl = lexy::dsl;

size_t GuessBlockSize(size_t size)
{
  auto root = static_cast<size_t>(std::floor(std::sqrt(static_cast<float>(size))));
  if(((size & (size - 1)) == 0) || // is power of 2
     (size % root == 0)) {
    return root;
  }

  for(size_t n = 1; n < root; ++n) {
    if(auto c = root - n; size % c == 0) {
      return c;
    }
    if(auto c = root + n; size % c == 0) {
      return c;
    }
  }
  throw std::logic_error("unexpected");
}

Args::Template MakeTemplate(lexy::nullopt)
{
  return Args::Template{9, 9, 3, 3};
}

Args::Template MakeTemplate(
  Args::Template::value_type width,
  lexy::nullopt,
  lexy::nullopt)
{
  auto blockSize = GuessBlockSize(width);
  return Args::Template{width, width, blockSize, blockSize};
}

Args::Template MakeTemplate(
  Args::Template::value_type width,
  Args::Template::value_type height,
  lexy::nullopt)
{
  return Args::Template{width, height, GuessBlockSize(width), GuessBlockSize(height)};
}

Args::Template MakeTemplate(
  Args::Template::value_type width,
  lexy::nullopt,
  Args::Template::value_type blockWidth,
  lexy::nullopt)
{
  return Args::Template{width, width, blockWidth, blockWidth};
}

Args::Template MakeTemplate(
  Args::Template::value_type width,
  Args::Template::value_type height,
  Args::Template::value_type blockWidth,
  lexy::nullopt)
{
  return Args::Template{width, height, blockWidth, GuessBlockSize(height)};
}

Args::Template MakeTemplate(
  Args::Template::value_type width,
  lexy::nullopt,
  Args::Template::value_type blockWidth,
  Args::Template::value_type blockHeight)
{
  return Args::Template{width, width, blockWidth, blockHeight};
}

Args::Template MakeTemplate(
  Args::Template::value_type width,
  Args::Template::value_type height,
  Args::Template::value_type blockWidth,
  Args::Template::value_type blockHeight)
{
  return Args::Template{width, height, blockWidth, blockHeight};
}

struct production
{
  // in argv_input there is no trailing separator; use while_ to match zero or multiple
  static constexpr auto sep = dsl::while_(dsl::argv_separator);

  struct flag
  {
    // just a flag defined by identifier literal; nothing else to parse
    static constexpr auto rule = sep;

    // always true if flag is given
    static constexpr auto value = lexy::constant(true);
  };

  struct template_
  {
    // =WxH,BW,BH is optional; if =W is given, xH and the rest is optional
    static constexpr auto rule = [] {
      auto dim = dsl::integer<Args::Template::value_type>;
      auto width_and_maybe_height = dim + dsl::opt(dsl::lit_c<'x'> >> dim);
      auto dims = width_and_maybe_height + dsl::opt(dsl::comma >> width_and_maybe_height);
      return dsl::opt(dsl::equal_sign >> dims) + sep;
    }();

    // dispatch to the appropriate constructor
    static constexpr auto value = lexy::callback<Args::Template>([](auto&&... args) { return MakeTemplate(args...); });
  };

  struct filepath
  {
    // a path and filename may include different characters incl. space
    static constexpr auto rule = dsl::identifier(dsl::ascii::print) + sep;

    static constexpr auto value = lexy::as_string<std::string>;
  };

  // identify arguments by conditions; partial_combination, as not all have to be given
  static constexpr auto rule = [] {
    auto make_arg = [](auto condition, auto rule) {
      return condition >> rule;
    };

    // if matched, the single hyphen is consumed
    auto single_hyphen = dsl::not_followed_by(dsl::hyphen, dsl::hyphen);

    // the negative no-hyphen rule has to take separator and end-of-input into account
    auto no_hyphen = dsl::peek_not(dsl::hyphen | dsl::argv_separator | dsl::eof);

    return dsl::partial_combination(
      make_arg(LEXY_LIT(ARG_HELP), LEXY_MEM(help) = dsl::p<flag>),
      make_arg(LEXY_LIT(ARG_TEMPLATE), LEXY_MEM(templ) = dsl::p<template_>),
      make_arg(single_hyphen, LEXY_MEM(stdinput) = dsl::p<flag>),
      make_arg(no_hyphen, LEXY_MEM(filepath) = dsl::p<filepath>)
    ) + dsl::eof;
  }();

  // [partial_]combination sets members by name as aggregate
  static constexpr auto value = lexy::as_aggregate<Args>;
};

} // namespace anonymous

std::ostream& operator<<(std::ostream& os, const Usage& u)
{
  os << "Usage: " << u.name << " [OPTION] [FILE]\n"
     << "  " ARG_HELP "                        Show this help.\n"
     << "  " ARG_TEMPLATE "[=W[xH][,BW[xBH]]]  Print a grid template of dimensions width x height fields of width x height blocks.\n"
     << "  FILE                          File path to read grid from. When FILE is -, read standard input.";
  return os;
}

Args ParseArgs(int argc, char** argv)
{
  auto input = lexy::argv_input(argc, argv);
  if(auto parsed = lexy::parse<production>(input, lexy_ext::report_error); parsed.has_value()) {
    auto&& args = parsed.value();
    if((args.help || args.templ || args.stdinput) && !args.filepath.empty()) {
      // the argv parser interprets trailing characters as filename
      // (e.g. "--helpmee" will be registered as flag "--help" and filepath "mee")
      // due to the way we work around the separator and partial_combination
      throw std::invalid_argument("invalid argument combination (or trailing characters)");
    }
    return args;
  }
  return Args{};
}

#undef ARG_HELP
#undef ARG_TEMPLATE
