#pragma once

#include "hypervector.h"

#include <optional>
#include <set>

struct Field
{
  using value_type = char;
  using Candidates = std::set<value_type>;

  static constexpr value_type undef = 0;

  std::optional<Candidates> candidates;
  value_type num;

  constexpr Field()
  : num(undef)
  {
  }

  constexpr Field(value_type num)
  : num(num)
  {
  }

  operator value_type() const noexcept
  {
    return num;
  }

  friend bool operator==(const Field& lhs, const Field& rhs) noexcept
  {
    return lhs.num == rhs.num;
  }

  friend bool operator!=(const Field& lhs, const Field& rhs) noexcept
  {
    return lhs.num != rhs.num;
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
