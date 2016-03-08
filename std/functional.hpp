
#ifndef STL_FUNCTIONAL_HPP
#define STL_FUNCTIONAL_HPP

#include "concepts.hpp"

#include <cstddef>
// For std::size_t.

#include <functional>
// For std::mem_fn.
//
// NOTE: This header dramatically slows build times.

namespace stl
{

// Invoke

// FIXME: Actually implement invoke.
template<typename F, typename... Args>
result_of_t<F(Args...)> invoke(F&& f, Args&&... args)
{
  return std::forward<F>(f)(std::forward<Args>(args)...);
}


// Identity function

struct identity_fn
{
  template<typename T>
  constexpr T& operator()(T&& t) const noexcept { return std::forward<T>(t); }
};


// Function objects
//
// TODO: Re-establish constraints on the primary template.

template<typename T = void>
struct equal_to;

template<EqualityComparable T>
struct equal_to<T>
{
  bool operator()(T const& a, T const& b) const { return a == b; }
};

template<>
struct equal_to<void>
{
  template<typename T, typename U>
    requires EqualityComparable<T, U>()
  bool operator()(T const& a, U const& b) const { return a == b; }
};


template<typename T = void>
struct not_equal_to;

template<EqualityComparable T>
struct not_equal_to<T>
{
  bool operator()(T const& a, T const& b) const { return a != b; }
};

template<>
struct not_equal_to<void>
{
  template<typename T, typename U>
    requires EqualityComparable<T, U>()
  bool operator()(T const& a, U const& b) const { return a != b; }
};


template<typename T = void>
struct less;

template<TotallyOrdered T>
struct less<T>
{
  bool operator()(T const& a, T const& b) const { return a < b; }
};

template<typename T>
struct less<T*>
{
  bool operator()(T const* a, T const* b) const { return a < b; }
};

template<>
struct less<void>
{
  template<typename T, typename U>
    requires TotallyOrdered<T, U>()
  bool operator()(T const& a, U const& b) const { return a < b; }

  template<typename T, typename U>
  bool operator()(T const* a, U const* b) const { return a < b; }
};


template<typename T = void>
struct greater;

template<TotallyOrdered T>
struct greater<T>
{
  bool operator()(T const& a, T const& b) const { return a > b; }
};

template<typename T>
struct greater<T*>
{
  bool operator()(T const* a, T const* b) const { return a > b; }
};

template<>
struct greater<void>
{
  template<typename T, typename U>
    requires TotallyOrdered<T, U>()
  bool operator()(T const& a, U const& b) const { return a > b; }

  template<typename T, typename U>
  bool operator()(T const* a, U const* b) const { return a > b; }
};


template<typename T = void>
struct less_equal;

template<TotallyOrdered T>
struct less_equal<T>
{
  bool operator()(T const& a, T const& b) const { return a <= b; }
};

template<typename T>
struct less_equal<T*>
{
  bool operator()(T const* a, T const* b) const { return a <= b; }
};

template<>
struct less_equal<void>
{
  template<typename T, typename U>
    requires TotallyOrdered<T, U>()
  bool operator()(T const& a, U const& b) const { return a <= b; }

  template<typename T, typename U>
  bool operator()(T const* a, U const* b) const { return a <= b; }
};


template<typename T = void>
struct greater_equal;

template<TotallyOrdered T>
struct greater_equal<T>
{
  bool operator()(T const& a, T const& b) const { return a >= b; }
};

template<typename T>
struct greater_equal<T*>
{
  bool operator()(T const* a, T const* b) const { return a > b; }
};

template<>
struct greater_equal<void>
{
  template<typename T, typename U>
    requires TotallyOrdered<T, U>()
  bool operator()(T const& a, U const& b) const { return a >= b; }

  template<typename T, typename U>
  bool operator()(T const* a, U const* b) const { return a >= b; }
};


} // namespace stl

#endif
