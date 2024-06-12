#pragma once

template<typename wrapped_iterator>
struct preserve_iterator : wrapped_iterator
{
  using value_type = wrapped_iterator;

  const wrapped_iterator& base() const noexcept
  {
    return static_cast<const wrapped_iterator&>(*this);
  }

  value_type operator*() const noexcept
  {
    return base();
  }
};

template<typename wrapped_iterator>
preserve_iterator<wrapped_iterator>
make_preserve_iterator(wrapped_iterator it)
{
  return preserve_iterator<wrapped_iterator>{it};
}
