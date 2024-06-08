#pragma once

#include "hypervector.h"

struct Field
{
  using value_type = unsigned char;

  static constexpr value_type undef = 0;

  value_type num;
};

// Block is 3x3 Fields
struct Grid : public hypervector<Field, 2> // row-major; width, height always in multiples of 3
{
  Grid() = default;

  Grid(size_t height, size_t width)
    : hypervector<Field, 2>(height, width)
  {
  }

  size_t height() const
  {
    return sizeOf<0>();
  }

  size_t width() const
  {
    return sizeOf<1>();
  }
};
