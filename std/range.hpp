
#ifndef STL_RANGE_HPP
#define STL_RANGE_HPP

#include "iterator.hpp"

namespace stl
{

// Range

// FIXME: The range specification seems incomplete (i.e., there's
// nothing there).
template<typename R>
concept bool Range()
{
  return requires {
    typename iterator_t<R>;
    typename sentinel_t<R>;
    requires Sentinel<sentinel_t<R>, iterator_t<R>>();
  };
}


// FIXME: Yuck.
template<typename R>
constexpr bool disable_sized_range = false;


// FIXME: Yuck. The remove_* complexity stems from the fact that
// ranges are generally forwarded into the concept.
template<typename R>
concept bool SizedRange()
{
  return Range<R>() &&
    !disable_sized_range<remove_cv_t<remove_reference_t<R>>> &&
    requires (remove_reference_t<R> const& r) {
      { size(r) } -> ConvertibleTo<difference_type_t<iterator_t<R>>>;
    };
}


template<typename R>
concept bool BoundedRange()
{
  return Range<R>() && SameAs<iterator_t<R>, sentinel_t<R>>();
}


// NOTE: Requirements on the Range can be weakened since they don't
// actually add requirements. That is, each of these concepts is a
// Range of some kind of iterator.

template<typename R>
concept bool InputRange()
{
  return Range<R>() && InputIterator<iterator_t<R>>();
}


template<typename R, typename T>
concept bool OutputRange()
{
  return Range<R>() && OutputIterator<iterator_t<R>>();
}


template<typename R>
concept bool ForwardRange()
{
  return Range<R>() && ForwardIterator<iterator_t<R>>();
}

template<typename R>
concept bool BidirectionalRange()
{
  return Range<R>() && BidirectionalIterator<iterator_t<R>>();
}


template<typename R>
concept bool RandomAccessRange()
{
  return Range<R>() && RandomAccessIterator<iterator_t<R>>();
}



// Range access
//
// TODO: This probably isn't right, but it gets around some immediate
// usability issues with the existing standard library. I'll need to
// build overloads specifically for the range concepts in this module.

using std::begin;
using std::end;

using std::cbegin;
using std::cend;

using std::rbegin;
using std::rend;

using std::crbegin;
using std::crend;

using std::size;
using std::empty;

using std::data;

// TODO: Not yet defined by GCC
// using std::cdata;



} // namespace stl

#endif
