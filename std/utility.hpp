
#ifndef STL_UTILITY_HPP
#define STL_UTILITY_HPP

#include "concepts.hpp"

#include <cstddef>


namespace stl
{

template<typename T>
struct is_nothrow_swappable
  : boolean_constant<
      is_nothrow_move_constructible_v<T> && is_nothrow_move_assignable_v<T>
    >
{ };

template<typename T>
using is_nothrow_swappable_t = typename is_nothrow_swappable<T>::type;

template<typename T>
constexpr bool is_nothrow_swappable_v = is_nothrow_swappable<T>::value;


// Swap

template<Movable T>
void swap(T& a, T& b)
{
  T t = std::move(a);
  a = std::move(b);
  b = std::move(t);
}


// TODO: Actually delgate to swap_ranges, and then optimize those
// cases.
template<Movable T, std::size_t N>
void swap(T (&a)[N], T (&b)[N]) noexcept(noexcept(swap(*a, *b)))
{
  for (T *p = a, *q = b; p != p + N; ++p, ++q)
    swap(*p, *q);
}


// Exchange

template<MoveConstructible T, typename U>
  requires Assignable<T&, U>()
T exchange(T& a, U&& b)
{
  T t = std::move(a);
  a = std::forward<U>(b);
  return t;
}


} // namespace stl

#endif
