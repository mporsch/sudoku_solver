#pragma once

#include "hypervector.h"

#include <vector>

// (parts of) the alphabet of the Sudoku; [1-9] in most cases but not necessarily
using Digit = char;
using Digits = std::vector<Digit>;

// cells of the Sudoku grid
struct Field
{
  static constexpr Digit undef = 0;

  Digit digit;

  constexpr Field()
  : digit(undef)
  {
  }

  constexpr Field(Digit digit)
  : digit(digit)
  {
  }

  bool HasValue() const noexcept
  {
    return (digit != undef);
  }

  operator Digit() const noexcept
  {
    return digit;
  }

  friend bool operator==(const Field& lhs, const Field& rhs) noexcept
  {
    return (lhs.digit == rhs.digit);
  }

  friend bool operator!=(const Field& lhs, const Field& rhs) noexcept
  {
    return (lhs.digit != rhs.digit);
  }

  friend bool operator<(const Field& lhs, const Field& rhs) noexcept
  {
    return (lhs.digit < rhs.digit);
  }
};

using Fields = std::vector<Field>;

template<typename T>
struct GridBase : public hypervector<T, 2> // row-major
{
  size_t blockHeight = 0;
  size_t blockWidth = 0;

  GridBase() = default;

  GridBase(
      size_t height, size_t width,
      size_t blockHeight, size_t blockWidth)
  : hypervector<T, 2>(height, width)
  , blockHeight(blockHeight)
  , blockWidth(blockWidth)
  {
  }

  GridBase(std::initializer_list<std::initializer_list<T>> init)
  : hypervector<T, 2>(std::move(init))
  , blockHeight(3)
  , blockWidth(3)
  {
  }

  size_t height() const
  {
    return this->template sizeOf<0>();
  }

  size_t width() const
  {
    return this->template sizeOf<1>();
  }

  friend bool operator==(const GridBase& lhs, const GridBase& rhs) noexcept
  {
    return true
    && (static_cast<const hypervector<Field, 2>&>(lhs) == static_cast<const hypervector<Field, 2>&>(rhs))
    && (lhs.blockHeight == rhs.blockHeight)
    && (lhs.blockWidth == rhs.blockWidth);
  }
};

struct Grid : public GridBase<Field>
{
  Grid() = default;

  Grid(
      size_t height, size_t width,
      size_t blockHeight, size_t blockWidth)
  : GridBase<Field>(height, width, blockHeight, blockWidth)
  {
  }

  Grid(std::initializer_list<std::initializer_list<Field>> init)
  : GridBase<Field>(std::move(init))
  {
    for(auto&& f : *this) {
      if(f.digit) {
        f.digit += 48; // upshift from integer to ASCII
      }
    }
  }
};
