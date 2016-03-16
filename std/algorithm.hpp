
#ifndef STL_ALGORITHM_HPP
#define STL_ALGORITHM_HPP

#include "iterator.hpp"
#include "range.hpp"


namespace stl
{

// All of

template<InputIterator I, Sentinel<I> S, IndirectPredicate<I> P>
bool
all_of(I first, S last, P pred)
{
  while (first != last) {
    if (!pred(*first))
      return false;
    ++first;
  }
  return true;
}

template<InputRange R, IndirectPredicate<iterator_t<R>> P>
bool
all_of(R&& range, P pred)
{
  return all_of(begin(range), end(range), pred);
}

template<typename T, Predicate<T> P>
inline bool
all_of(std::initializer_list<T> list, P pred)
{
  return all_of(list.begin(), list.end(), pred);
}

// All of (projected)

template<InputIterator I, Sentinel<I> S, typename P, typename X>
  requires IndirectPredicate<P, projected<I, X>>()
bool
all_of(I first, S last, P pred, X proj)
{
  while (first != last) {
    if (!pred(proj(*first)))
      return false;
    ++first;
  }
  return true;
}

template<InputRange R, typename P, typename X>
  requires IndirectPredicate<P, projected<iterator_t<R>, X>>()
bool
all_of(R&& range, P pred, X proj)
{
  return all_of(begin(range), end(range), pred, proj);
}

template<typename T, typename P, typename X>
  requires Predicate<P, result_of_t<X(T)>>()
inline bool
all_of(std::initializer_list<T> list, P pred, X proj)
{
  return all_of(list.begin(), list.end(), pred, proj);
}

} // namespace stl

#endif
