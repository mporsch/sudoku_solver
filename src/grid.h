#pragma once

#include "hypervector.h"

struct Field
{
  using value_type = unsigned char;

  static constexpr value_type undef = 0;

  value_type num;

  Field()
  : num(undef)
  {
  }

  Field(value_type num)
  : num(num)
  {
  }

  friend bool operator==(const Field& lhs, const Field& rhs) noexcept
  {
    return lhs.num == rhs.num;
  }

  friend bool operator<(const Field& lhs, const Field& rhs) noexcept
  {
    return lhs.num < rhs.num;
  }
};

struct Grid : public hypervector<Field, 2> // row-major; width, height always in multiples of 3
{
  static constexpr size_t BlockSize = 3;

  Grid() = default;

  Grid(size_t height, size_t width)
    : hypervector<Field, 2>(height * BlockSize, width * BlockSize)
  {
  }

  Grid(std::initializer_list<Field> init)
    : hypervector<Field, 2>(std::move(init))
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
