#include "args_parse.h"

#include "lexy/action/parse.hpp"
#include "lexy/callback.hpp"
#include "lexy/dsl.hpp"
#include "lexy/input/argv_input.hpp"
#include "lexy_ext/report_error.hpp"

#define ARG_HELP "--help"
#define ARG_TEMPLATE "--template"

namespace {

namespace dsl = lexy::dsl;

Args::Template MakeTemplate(lexy::nullopt)
{
  return Args::Template{9, 9};
}
Args::Template MakeTemplate(Args::Template::value_type width, lexy::nullopt)
{
  return Args::Template{width, width};
}
Args::Template MakeTemplate(Args::Template::value_type width, Args::Template::value_type height)
{
  return Args::Template{width, height};
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
    // =WxH is optional; if =W is given, xH is optional
    static constexpr auto rule = [] {
      auto dim = dsl::integer<Args::Template::value_type>;
      auto width_and_maybe_height = dim + dsl::opt(dsl::lit_c<'x'> >> dim);
      return dsl::opt(dsl::lit_c<'='> >> width_and_maybe_height) + sep;
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
  os << "Usage: " << u.name << "\n"
     << "  [" ARG_HELP "]              Show this help.\n"
     << "  [" ARG_TEMPLATE "[=W[xH]]]  Print a template of dimensions width x height blocks.\n"
     << "  [FILE]                File path to read grid from. When FILE is -, read standard input.";
  return os;
}

Args ParseArgs(int argc, char** argv)
{
  auto input = lexy::argv_input(argc, argv);
  if(auto parsed = lexy::parse<production>(input, lexy_ext::report_error); parsed.has_value()) {
    return parsed.value();
  }
  return Args{};
}

#undef ARG_HELP
#undef ARG_TEMPLATE
