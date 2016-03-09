
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



// Range access
//
// TODO: This probably isn't right, but it gets around some immediate
// usability issues with the existing standard library.
using std::begin;
using std::end;



} // namespace stl

#endif
