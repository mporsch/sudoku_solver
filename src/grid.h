#pragma once

#include "hypervector.h"

struct Field
{
  using value_type = char;

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

struct Grid : public hypervector<Field, 2> // row-major
{
  size_t blockHeight = 0;
  size_t blockWidth = 0;

  Grid() = default;

  Grid(size_t height, size_t width,
       size_t blockHeight, size_t blockWidth)
    : hypervector<Field, 2>(height, width)
    , blockHeight(blockHeight)
    , blockWidth(blockWidth)
  {
  }

  Grid(std::initializer_list<std::initializer_list<Field>> init)
    : hypervector<Field, 2>(std::move(init))
    , blockHeight(3)
    , blockWidth(3)
  {
    for(auto&& f : *this) {
      if(f.num) {
        f.num += 48; // upshift from integer to ASCII
      }
    }
  }

  size_t height() const
  {
    return sizeOf<0>();
  }

  size_t width() const
  {
    return sizeOf<1>();
  }

  friend bool operator==(const Grid& lhs, const Grid& rhs) noexcept
  {
    return true
    && (static_cast<const hypervector<Field, 2>&>(lhs) == static_cast<const hypervector<Field, 2>&>(rhs))
    && (lhs.blockHeight == rhs.blockHeight)
    && (lhs.blockWidth == rhs.blockWidth);
  }
};
