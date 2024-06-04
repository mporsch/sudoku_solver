#pragma once

#include "hypervector.h"

struct Field
{
  using value_type = unsigned char;

  static constexpr value_type undef = 0;

  value_type num;
};

// Block is 3x3 Fields
using Grid = hypervector<Field, 2>; // row-major; width, height always in multiples of 3
