
#ifndef STL_CONCEPTS_HPP
#define STL_CONCEPTS_HPP

#include "traits.hpp"

namespace stl
{

template<typename T, typename U>
concept bool SameAs()
{
  return __is_same_as(T, U);
}


template<typename T, typename U>
concept bool DerivedFrom()
{
  return __is_base_of(U, T);
}


template<typename T, typename U>
concept bool ConvertibleTo()
{
  return is_convertible_v<T, U>;
}


// TODO: Find a way to simplify this. Also, CommonWith?
template<typename T, typename U>
concept bool Common()
{
  return requires (T t, U u) {
    typename common_type_t<T, U>;
    typename common_type_t<U, T>;
    requires SameAs<common_type_t<T, U>, common_type_t<U, T>>();
    common_type_t<T, U>(t);
    common_type_t<U, T>(u);
  };
}


// FIXME: This might not be a good concept.
template<typename T>
concept bool Integral()
{
  return is_integral_v<T>;
}


template<typename T>
concept bool SignedIntegral()
{
  return Integral<T>() && is_signed_v<T>;
}


template<typename T>
concept bool UnignedIntegral()
{
  return Integral<T>() && is_unsigned_v<T>;
}


namespace impl
{

using std::swap;

// FIXME: The TS uses && for a and b. I'm not sure that this is
// desirable since. The template argument T should be an object
// type (but possibly a function type?).
//
// Note that the current design makes it impossible to swap rvalue
// references. Re-read the TS and figure out what is actually intended.
template<typename T>
concept bool Swappable()
{
  return requires (T& a, T& b) {
    swap(a, b);
  };
}

template<typename T, typename U>
concept bool Swappable()
{
  return Swappable<T>() && Swappable<U>() && Common<T, U>() &&
    requires(T& a, U& b) {
      swap(a, b);
      swap(a, b);
    };
}

} // namespace impl


// FIXME: This might be misplaced. Locate nearer to the algorithm.
template<typename T>
concept bool Swappable()
{
  return impl::Swappable<T>();
}


template<typename T, typename U>
concept bool Swappable()
{
  return impl::Swappable<T, U>();
}


// FIXME: This isn't right. Why do we allow this kind of generalization
template<typename T>
concept bool Boolean()
{
  return SameAs<T, bool>();
}


template<typename T>
concept bool EqualityComparable()
{
  return requires (T a, T b) {
    { a == b } -> Boolean;
    { a != b } -> Boolean;
  };
}


// FIXME: Rename this EqualityComparable and ensure that T and
// U are also equality comparble. But skip the common type bits.
template<typename T, typename U>
concept bool WeaklyEqualityComparable()
{
  return requires (T a, U b) {
    { a == b } -> Boolean;
    { b == a } -> Boolean;
    { a != b } -> Boolean;
    { b != a } -> Boolean;
  };
}


template<typename T, typename U>
concept bool EqualityComparable()
{
  return Common<T, U>()
      && EqualityComparable<T>()
      && EqualityComparable<U>()
      && EqualityComparable<common_type_t<T, U>>()
      && WeaklyEqualityComparable<T, U>();
}


template<typename T>
concept bool TotallyOrdered()
{
  return EqualityComparable<T>() && requires (T a, T b) {
    { a < b } -> Boolean;
    { a > b } -> Boolean;
    { a <= b } -> Boolean;
    { a >= b } -> Boolean;
  };
}


template<typename T, typename U>
concept bool TotallyOrdered()
{
  return requires (T a, U b) {
    { a < b } -> Boolean;
    { b < a } -> Boolean;
    { a > b } -> Boolean;
    { b > a } -> Boolean;
    { a <= b } -> Boolean;
    { b <= a } -> Boolean;
    { a >= b } -> Boolean;
    { b >= a } -> Boolean;
  };
}


// Variable concepts

// NOTE: The requirement for is_destructible_v ensures that not every
// object type will be a variable type. This is somewhat counterintuitive,
// but I suspect it is correct. If you can declare a variable, it must
// be able to go out of scope. If you know that something is an object
// type, then you know only that it's object representaiton can occupy
// storage.
template<typename T>
concept bool Variable()
{
  return (is_object_v<T> || is_reference_v<T>) && is_destructible_v<T>;
}


// Can be directly initialized by a sequence of arguments. Any type
// that can be initialized is implicitly destroyed when it goes out
// of scope (e.g., references).
template<typename T, typename... Args>
concept bool Initializable()
{
  return Variable<T>() && requires (int n, Args&&... args) {
    T(std::forward<Args>(args)...);
  };
}


// FIXME: This almost certainly an object concept, but sometimes
// we actually want "assignable through".
template<typename T, typename U>
concept bool Assignable()
{
  return Common<T, U>() && requires (T& a, U&& b)
  {
    { std::forward<T>(a) = std::forward<U>(b) } -> T&; // FIXME: == T&
  };
}


// Object concepts

// All of these concepts take a primary type argument. This must
// be an object type, but it may be a constant type.

template<typename T>
concept bool Object()
{
  return is_object_v<T>;
}


template<typename T>
concept bool Destructible()
{
  return is_object_v<T> && requires (T t, T const ct, T* p) {
    { t.~T() } noexcept;
    { &t } -> SameAs<T*>;
    { &ct } -> SameAs<T const*>;
    delete p;
    delete[] p;
  };
}


template<typename T, typename... Args>
concept bool Constructible()
{
  return Destructible<T>() &&
         Initializable<T, Args...>() &&
  requires (std::size_t n, Args&&... args) {
    { new T(std::forward<Args>(args)...) } -> T*;
  };
}


template<typename T>
concept bool DefaultConstructible()
{
  return Constructible<T>() && requires (int n) {
    { new T[n]{} } -> T*;
  };
}


template<typename T>
concept bool MoveConstructible()
{
  return Constructible<T, remove_cv_t<T>&&>();
}


template<typename T>
concept bool CopyConstructible()
{
  return MoveConstructible<T>() && Constructible<T, remove_cv_t<T> const&>();
}


// FIXME: Move assignemnt can only be performed on object types.
template<typename T>
concept bool MoveAssignable()
{
  return Object<T>() && Assignable<T&, T&&>();
}


template<typename T>
concept bool CopyAssignable()
{
  return MoveAssignable<T>() && Assignable<T&, T const&>();
}


template<typename T>
concept bool Movable()
{
  return MoveConstructible<T>() && MoveAssignable<T>();
}


template<typename T>
concept bool Copyable()
{
  return MoveConstructible<T>() && MoveAssignable<T>();
}


template<typename T>
concept bool Semiregular()
{
  return DefaultConstructible<T>() && Copyable<T>();
}


template<typename T>
concept bool Regular()
{
  return Semiregular<T>() && EqualityComparable<T>();
}


// Function concepts


template<typename F, typename... Args>
concept bool Callable()
{
  return requires (F f, Args&&... args) {
    invoke(f, std::forward<Args>(args)...);
  };
}


template<typename F, typename... Args>
concept bool RegularFunction()
{
  return Callable<F, Args...>();
}


template<typename P, typename... Args>
concept bool Predicate()
{
  return RegularFunction<P, Args...>() && Boolean<result_of_t<P(Args...)>>();
}


template<typename R, typename T>
concept bool Relation()
{
  return Predicate<R, T, T>();
}


template<typename R, typename T, typename U>
concept bool Relation()
{
  return Relation<R, T>()
      && Relation<R, U>()
      && Common<T, U>()
      && Relation<R, common_type_t<T, U>>()
      && Predicate<R, T, U>()
      && Predicate<R, U, T>();
}


template<typename R, typename T>
concept bool EquivalenceRelation()
{
  return Relation<R, T>();
}


template<typename R, typename T, typename U>
concept bool EquivalenceRelation()
{
  return Relation<R, T, U>();
}


template<typename R, typename T>
concept bool StrictWeakOrder()
{
  return Relation<R, T>();
}


template<typename R, typename T, typename U>
concept bool StrictWeakOrder()
{
  return Relation<R, T, U>();
}


// General properties


template<typename T>
concept bool
has_nested_value_type()
{
  return requires { typename T::value_type; }
      && Object<typename T::value_type>();
}

template<typename T>
concept bool
has_nested_element_type()
{
  return requires { typename T::element_type; }
      && Object<typename T::element_type>();
}


// Value type

template<typename T>
struct value_type;

template<typename T>
struct value_type<T*>
{
  using type = T;
};

template<typename T>
struct value_type<T const*>
{
  using type = T;
};

template<typename T>
  requires has_nested_value_type<T>()
struct value_type<T>
{
  using type = typename T::value_type;
};

template<typename T>
  requires has_nested_element_type<T>()
struct value_type<T>
{
  using type = typename T::element_type;
};

template<typename T>
using value_type_t = typename value_type<T>::type;


// Reference type

template<typename T>
  requires requires (T t) { *t; }
using reference_t = decltype(*std::declval<T>());


// Pointer type
//
// NOTE: This isn't required... I don't really know what the TS
// is doing with pointer types.

template<typename T>
  requires requires (T t) { &*t; }
using pointer_t = decltype(&*std::declval<T>());


// Difference type

template<typename T>
struct difference_type;

template<typename T>
struct difference_type<T*>
{
  using type = std::ptrdiff_t;
};

template<typename T>
  requires requires { typename T::difference_type; }
struct difference_type<T>
{
  using type = typename T::difference_type;
};

template<typename T>
using difference_type_t = typename difference_type<T>::type;


// Rvalue reference type

template<typename T>
using rvalue_reference_t = decltype(std::move(std::declval<reference_t<T>>()));


// Iterator type

template<typename T>
using iterator_t = decltype(begin(std::declval<T&>()));


// Sentinel type

template<typename T>
using sentinel_t = decltype(end(std::declval<T&>()));


} // namespace stl

#endif
