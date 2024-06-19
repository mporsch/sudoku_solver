#pragma once

#include <cassert>

// TODO past-the-'past-the-end' iterators are considered UB
template<typename wrapped_iterator>
struct custom_step_iterator : wrapped_iterator
{
  using typename wrapped_iterator::difference_type;
  using typename wrapped_iterator::reference;

  difference_type step;

  const wrapped_iterator& base() const noexcept
  {
    return static_cast<const wrapped_iterator&>(*this);
  }

  wrapped_iterator& operator++() noexcept
  {
    static_cast<wrapped_iterator&>(*this) += step;
    return *this;
  }

  wrapped_iterator operator++(int) noexcept
  {
    auto tmp = *this;
    static_cast<wrapped_iterator&>(*this) += step;
    return tmp;
  }

  wrapped_iterator& operator--() noexcept
  {
    static_cast<wrapped_iterator&>(*this) -= step;
    return *this;
  }

  wrapped_iterator operator--(int) noexcept
  {
    auto tmp = *this;
    static_cast<wrapped_iterator&>(*this) -= step;
    return tmp;
  }

  wrapped_iterator& operator+=(difference_type n) noexcept
  {
    static_cast<wrapped_iterator&>(*this) += step * n;
    return *this;
  }

  wrapped_iterator operator+(difference_type n) const noexcept
  {
    return wrapped_iterator(base() + step * n, step);
  }

  wrapped_iterator& operator-=(difference_type n) noexcept
  {
    static_cast<wrapped_iterator&>(*this) -= step * n;
    return *this;
  }

  wrapped_iterator operator-(difference_type n) const noexcept
  {
    return wrapped_iterator(base() - step * n, step);
  }

  reference operator[](difference_type n) const noexcept
  {
    return base()[step * n];
  }

  friend difference_type operator-(
    const custom_step_iterator& lhs,
    const custom_step_iterator& rhs) noexcept
  {
    auto diff = lhs.base() - rhs.base();
    assert((lhs.step == rhs.step) && (diff % lhs.step == 0));
    return diff / lhs.step;
  }
};

template<typename wrapped_iterator>
custom_step_iterator<wrapped_iterator>
make_custom_step_iterator(
  wrapped_iterator it,
  typename wrapped_iterator::difference_type step)
{
  return custom_step_iterator<wrapped_iterator>{it, step};
}
