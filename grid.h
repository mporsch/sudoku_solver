#pragma once

#include "hypervector.h"

#include "lexy/dsl/option.hpp"

#include <stdexcept>

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

// Block is 3x3 Fields
using Grid = hypervector<Field, 2>; // row-major; width, height always in multiples of 3
