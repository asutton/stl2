
#ifndef STL_ALGORITHM_HPP
#define STL_ALGORITHM_HPP

#include "iterator.hpp"


namespace stl
{

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

template<typename T, Predicate<T> P>
inline bool
all_of(std::initializer_list<T> list, P pred)
{
  return all_of(list.begin(), list.end(), pred);
}

template<InputIterator I, Sentinel<I> S, typename P, typename X = identity_fn>
  requires IndirectPredicate<P, projected<I, X>>()
bool all_of(I first, S last, P pred, X proj = {})
{
  while (first != last) {
    if (!pred(proj(*first)))
      return false;
    ++first;
  }
  return true;
}

} // namespace stl

#endif
