#pragma once

#include <deque>
#include <stdexcept>

template<typename wrapped_iterator>
struct dance_dance_iterator : wrapped_iterator
{
  using iterator_category = std::bidirectional_iterator_tag;

  std::deque<typename wrapped_iterator::difference_type> steps;

  const wrapped_iterator& base() const noexcept {
    return static_cast<const wrapped_iterator&>(*this);
  }

  wrapped_iterator& operator++() {
    static_cast<wrapped_iterator&>(*this) += forward();
    return *this;
  }

  wrapped_iterator operator++(int) {
    auto tmp = *this;
    static_cast<wrapped_iterator&>(*this) += forward();
    return tmp;
  }

  wrapped_iterator& operator--() {
    static_cast<wrapped_iterator&>(*this) -= backward();
    return *this;
  }

  wrapped_iterator operator--(int) {
    auto tmp = *this;
    static_cast<wrapped_iterator&>(*this) -= backward();
    return tmp;
  }

  friend bool operator!=(const dance_dance_iterator& lhs, const dance_dance_iterator& rhs) noexcept {
    return false
    ||  (static_cast<const wrapped_iterator&>(lhs) != static_cast<const wrapped_iterator&>(rhs))
    ||  (lhs.steps != rhs.steps);
  }

private:
  typename wrapped_iterator::difference_type forward() {
    if(steps.empty()) {
      throw std::runtime_error("no more steps");
    }
    auto step = steps.front();
    steps.pop_front();
    return step;
  }

  typename wrapped_iterator::difference_type backward() {
    if(steps.empty()) {
      throw std::runtime_error("no more steps");
    }
    auto step = steps.back();
    steps.pop_back();
    return step;
  }
};

template<typename wrapped_iterator, typename... Steps>
dance_dance_iterator<wrapped_iterator> make_dance_dance_iterator(wrapped_iterator it, Steps&&... steps)
{
  return dance_dance_iterator<wrapped_iterator>{it,
    std::deque<typename wrapped_iterator::difference_type>{
      static_cast<typename wrapped_iterator::difference_type>(steps)...}};
}
