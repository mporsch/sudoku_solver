#pragma once

#include "lexy/action/parse.hpp"
#include "lexy/callback.hpp"
#include "lexy/dsl.hpp"
#include "lexy/input/argv_input.hpp"
#include "lexy_ext/report_error.hpp"

#include <optional>

struct Args
{
  struct Template
  {
    using value_type = size_t;

    value_type width;
    std::optional<value_type> height;

    constexpr Template(lexy::nullopt)
      : width(9)
      , height(9)
    {
    }

    constexpr Template(value_type width, lexy::nullopt)
      : width(width)
      , height(width)
    {
    }

    constexpr Template(value_type width, value_type height)
      : width(width)
      , height(height)
    {
    }
  };

  std::optional<bool> help;
  std::optional<Template> templ;
};

namespace arg_parse_detail {

namespace dsl = lexy::dsl;

struct production
{
  struct help
  {
    static constexpr auto rule = dsl::eol;

    static constexpr auto value = lexy::constant(true);
  };

  struct template_
  {
    static constexpr auto rule = [] {
      auto dim = dsl::integer<Args::Template::value_type>;
      auto width_and_maybe_height = dim + dsl::opt(dsl::lit_c<'x'> >> dim);
      return dsl::opt(dsl::lit_c<'='> >> width_and_maybe_height) + dsl::eol;
    }();

    static constexpr auto value = lexy::construct<Args::Template>;
  };

  static constexpr auto rule = [] {
    auto make_arg = [](auto name, auto rule) {
      return name >> rule;
    };

    auto arg_help = make_arg(LEXY_LIT("--help"), LEXY_MEM(help) = dsl::p<help>);
    auto arg_template = make_arg(LEXY_LIT("--template"), LEXY_MEM(templ) = dsl::p<template_>);

    return dsl::combination(arg_help, arg_template) + dsl::eof;
  }();

  static constexpr auto value = lexy::as_aggregate<Args>;
};

} // namespace arg_parse_detail

Args ParseArgs(int argc, char** argv)
{
  using namespace arg_parse_detail;

  auto input = lexy::argv_input(argc, argv);
  if(auto parsed = lexy::parse<production>(input, lexy_ext::report_error); parsed.has_value()) {
    return parsed.value();
  }
  return Args{};
}
