
#ifndef STL_ITERATOR_HPP
#define STL_ITERATOR_HPP

#include "concepts.hpp"
#include "functional.hpp" // for mem_fn

#include <cstddef>
#include <iterator>


namespace stl
{

template<typename T>
concept bool has_nested_value_type()
{
  return requires { typename T::value_type; }
      && Object<typename T::value_type>();
}

template<typename T>
concept bool has_nested_element_type()
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


// Rvalue reference type

template<typename T>
using rvalue_reference_t = decltype(std::move(std::declval<reference_t<T>>()));


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
struct difference_type<T*>
{
  using type = typename T::difference_type;
};

template<typename T>
using difference_type_t = typename difference_type<T>::type;


// Iterator category

template<typename T>
struct iterator_category;

template<typename T>
struct iterator_category<T*>
{
  using type = std::random_access_iterator_tag;
};

template<typename T>
  requires requires { typename T::iterator_category; }
struct iterator_category<T>
{
  using type = typename T::iterator_category;
};

template<typename T>
using iterator_category_t = typename iterator_category<T>::type;


// Concepts

template<typename I>
concept bool Readable()
{
  return Semiregular<I>() && requires (const I i) {
    { *i } -> const value_type_t<I>&;
  };
}


template<typename O, typename T>
concept bool Writable()
{
  return Semiregular<O>() && requires (O o, T&& v) {
    *o = std::forward<T>(v);
  };
}


template<typename I>
concept bool WeaklyIncrementable()
{
  return Semiregular<I>() && requires (I i) {
    typename difference_type_t<I>;
    requires SignedIntegral<difference_type_t<I>>();
    { ++i } -> SameAs<I&>;
    i++;
  };
}


template<typename I>
concept bool Incrementable()
{
  return Regular<I>() && WeaklyIncrementable<I>() && requires (I i) {
    { i++ } -> SameAs<I>;
  };
}


// FIXME: Iterators have lost equality comparison. In fact, so have
// incrementable types. I can buy that weakly incrementable, and
// therefore input/output iterators can be weakly equality comparable,
// but that must not be the case for forward iterators. They should
// be regular types.
template<typename I>
concept bool Iterator()
{
  return WeaklyIncrementable<I>() && requires (I i) {
    { *i } -> auto&&;
  };
}


// FIXME: Why can I increment a sentinel?
template<typename S, typename I>
concept bool Sentinel()
{
  return Semiregular<S>() && Iterator<I>() && WeaklyEqualityComparable<S, I>();
}


template<typename T>
constexpr bool disable_sized_sentinel = false;


// FIXME: Yuck.
template<typename S, typename I>
concept bool SizedSentinel()
{
  return Sentinel<S, I>() &&
    !disable_sized_sentinel<remove_cv_t<S>, remove_cv_t<I>> &&
    requires (const I i, const S s) {
      { s - i } -> SameAs<difference_type_t<I>>;
      { i - s } -> SameAs<difference_type_t<I>>;
    };
}


template<typename I>
concept bool InputIterator()
{
  return Iterator<I>() &&
    Readable<I>() &&
    requires(I i, const I ci) {
      typename iterator_category_t<I>;
      requires DerivedFrom<iterator_category_t<I>, std::input_iterator_tag>();
      { *ci } -> const value_type_t<I>&;
      { i++ } -> Readable;
      { *i++ } -> const value_type_t<I>&;
      //  requires Same<value_type_t<I>, value_type_t<decltype(i++)>>();
    };
}


template<typename I, typename T>
concept bool OutputIterator()
{
  return Iterator<I>() && Writable<I, T>();
}


template<typename I>
concept bool ForwardIterator()
{
  return InputIterator<I>() &&
    Incrementable<I>() &&
    DerivedFrom<iterator_category_t<I>, std::forward_iterator_tag>() &&
    Sentinel<I, I>(); // FIXME: This cannot be right.
};


template<typename I>
concept bool BidirectionalIterator()
{
  return ForwardIterator<I>() &&
    DerivedFrom<iterator_category_t<I>, std::bidirectional_iterator_tag>() &&
    requires (I i) {
      { --i } -> SameAs<I&>;
      { i-- } -> SameAs<I>;
    };
}


template<typename I>
concept bool RandomAccessIterator()
{
  return BidirectionalIterator<I>() &&
    TotallyOrdered<I>() &&
    SizedSentinel<I>() && // FIXME: Not if iterators aren't sentinels.
    DerivedFrom<iterator_category_t<I>, std::random_access_iterator_tag>() &&
    requires (I i, I j, difference_type_t<I> n) {
      { i += n } -> SameAs<I&>;
      { j + n } -> SameAs<I>;
      { n + j } -> SameAs<I>;
      { i -= n } -> SameAs<I&>;
      { j - n } -> SameAs<I>;
      { j[n] } -> SameAs<reference_t<I>>;
    };
}


// As function

template<typename T>
  requires is_member_pointer_v<decay_t<T>>
auto get_as_function(T&& t)
{
  return std::mem_fn(t);
}

template<typename T>
T get_as_function(T&& t)
{
  return std::forward<T>(t);
}

template<typename T>
using as_function_t = decltype(get_as_function(std::declval<T>()));


// Callable concepts

template<typename F, Readable... Iters>
concept bool IndirectlyCallable()
{
  return Callable<F, value_type_t<Iters>...>();
}


template<typename F, Readable... Iters>
concept bool IndirectRegularFunction()
{
  return RegularFunction<F, value_type_t<Iters>...>();
}


template<typename F, Readable... Iters>
concept bool IndirectPredicate()
{
  return Predicate<F, value_type_t<Iters>...>();
}


template<typename F, Readable I1, Readable I2>
concept bool IndirectRelation()
{
  return Relation<F, value_type_t<I1>, value_type_t<I2>>;
}


template<typename F, Readable... Iters>
concept bool IndirectEquivalenceRelation()
{
  return EquivalenceRelation<F, value_type_t<Iters>...>();
}


template<typename F, Readable I1, Readable I2>
concept bool IndirectStrictWeakOrder()
{
  return StrictWeakOrder<F, value_type_t<I1>, value_type_t<I2>>;
}


// Indirect result of

template<typename T>
struct indirect_result_of;

template<typename F, typename... Iters>
  requires IndirectlyCallable<remove_reference_t<F>, Iters...>()
struct indirect_result_of<F(Iters...)> : result_of_t<F(value_type<Iters>...)>
{ };

template<typename C>
using indirect_result_of_t = typename indirect_result_of<C>::type;


// Projected

template<Readable I, IndirectRegularFunction<I> P>
  requires RegularFunction<P, reference_t<I>>()
struct projected
{
  using value_type = decay_t<indirect_result_of_t<P&(I)>>;

  result_of_t<P&(reference_t<I>)> operator*() const;
};


// Indrect move, copy, and transfer

template<typename I, typename O>
concept bool IndirectlyMovable()
{
  return Readable<I>() && Writable<O, rvalue_reference_t<I>>();
}


// FIXME: Wht reference and not const reference?
template<typename I, typename O>
concept bool IndirectlyCopyable()
{
  return IndirectlyMovable<I> && Writable<O, reference_t<I>>();
}


// FIXME: We get this kind of concept because we don't add basic
// guarantees to the value type of readable objects. I wonder if,
// minimally, those should all be movable. What examples do we
// have where this is not the case? In either case, this must
// improve.
template<typename I, typename O>
concept bool IndirectlyMovableViaTemporary()
{
  return IndirectlyMovable<I, O>() &&
    Writable<O, value_type_t<I>&&>() &&
    Movable<value_type_t<I>>() &&
    Constructible<value_type_t<I>, rvalue_reference_t<I>>() &&
    Assignable<value_type_t<I>&, rvalue_reference_t<I>>();
}


template<typename I, typename O>
concept bool IndrectlyCopyableViaTemporary()
{
  return IndirectlyMovableViaTemporary<I, O>() &&
    IndirectlyCopyable<I, O>() &&
    Writable<O, value_type<I> const&>() && // FIXME: Needed?
    Copyable<value_type<I>>() &&
    Constructible<value_type<I>, reference_t<I>>() &&
    Assignable<value_type<I>&, reference_t<I>>();
}


template<typename I1, typename I2 = I1>
concept bool IndirectlySwappable()
{
  return Readable<I1>() && Readable<I2>() &&
    Swappable<reference_t<I1>, reference_t<I2>>();
}


template<typename I1, typename I2, typename R = equal_to<>,
         typename P1 = identity_fn, typename P2 = identity_fn>
concept bool IndirectlyComparable()
{
  return IndirectRelation<R, projected<I1, P1>, projected<I2, P2>>();
}


// Algorithm concepts

template<typename I>
concept bool Permutable()
{
  return ForwardIterator<I>() &&
    IndirectlyMovableViaTemporary<I, I>() &&
    IndirectlySwappable<I, I>();
}


template<typename I1, typename I2, typename O, typename R = less<>,
         typename P1 = identity_fn, typename P2 = identity_fn>
concept bool Mergeable()
{
  return InputIterator<I1>() && InputIterator<I2>() && WeaklyIncrementable<O> &&
         IndirectlyCopyable<I1, O> && IndirectlyCopyable<I2, O> &&
         IndirectStrictWeakOrder<R, projected<I1, P1>, projected<I2, P2>>();
}


template<typename I, typename R = less<>, typename P = identity_fn>
concept bool Sortable()
{
  return Permutable<I>() && IndirectStrictWeakOrder<R, projected<I, P>>();
}

// Advance

template<Iterator I>
void advance(I& iter, difference_type_t<I> n = 1)
{
  while (n != 0) { ++iter; --n; }
}

template<BidirectionalIterator I>
void advance(I& iter, difference_type_t<I> n = 1)
{
  if (n > 0)
    while (n != 0) { ++iter; --n; }
  else
    while (n != 0) { --iter; ++n; }
}

template<RandomAccessIterator I>
void advance(I& iter, difference_type_t<I> n = 1)
{
  iter += n;
}

// FIXME: I don't understand the assignable bits in the specification.
//
// TODO: Specialize for sized sentinels.
template<Iterator I, Sentinel<I> S>
void advance(I& iter, S bound)
{
  while (iter != bound)
    ++iter;
}

// TODO: Optimize for sized sentinels.
template<Iterator I, Sentinel<I> S>
void advance(I& iter, difference_type_t<I> n, S bound)
{
  while (n != 0 && iter != bound) { ++iter; --n; }
}

template<BidirectionalIterator I, Sentinel<I> S>
void advance(I& iter, difference_type_t<I> n, S bound)
{
  if (n > 0)
    while (n != 0 && iter != bound) { ++iter; --n; }
  else
    while (n != 0 && iter != bound) { --iter; ++n; }
}


// Next

template<Iterator I>
I next(I& iter, difference_type_t<I> n = 1)
{
  advance(iter, n);
  return iter;
}

template<Iterator I, Sentinel<I> S>
I next(I& iter, S bound)
{
  advance(iter, bound);
  return iter;
}

template<Iterator I, Sentinel<I> S>
I next(I& iter, difference_type_t<I> n, S bound)
{
  advance(iter, n, bound);
  return iter;
}


// Prev

template<BidirectionalIterator I>
I prev(I& iter, difference_type_t<I> n = 1)
{
  advance(iter, -n);
  return iter;
}

template<BidirectionalIterator I, Sentinel<I> S>
I prev(I& iter, difference_type_t<I> n, S bound)
{
  advance(iter, -n, bound);
  return iter;
}


// Reverse iterator

template<BidirectionalIterator I>
struct reverse_iterator
{
  using iterator_type     = I;
  using value_type        = value_type_t<I>;
  using reference         = reference_t<I>;
  using difference_type   = difference_type_t<I>;
  using iterator_category = iterator_category_t<I>;

  reverse_iterator() = default;
  reverse_iterator(reverse_iterator const&) = default;
  reverse_iterator& operator=(reverse_iterator const&) = default;

  explicit reverse_iterator(I i)
    : iter(i)
  { }

  I base() const { return iter; }
  reference operator*() const;
  I operator->() const { return prev(iter); }

  reverse_iterator& operator++();
  reverse_iterator& operator--();
  reverse_iterator  operator++(int);
  reverse_iterator  operator--(int);

  reverse_iterator& operator+=(difference_type) requires RandomAccessIterator<I>();
  reverse_iterator operator+(difference_type) requires RandomAccessIterator<I>();

  reverse_iterator& operator-=(difference_type) requires RandomAccessIterator<I>();
  reverse_iterator operator-(difference_type) requires RandomAccessIterator<I>();

  reference operator[](difference_type n) requires RandomAccessIterator<I>();

  I iter;
};

template<BidirectionalIterator I>
inline auto
reverse_iterator<I>::operator*() const -> reference
{
  return *prev(iter);
}

template<BidirectionalIterator I>
inline auto
reverse_iterator<I>::operator++() -> reverse_iterator&
{
  --iter;
  return *this;
}

template<BidirectionalIterator I>
inline auto
reverse_iterator<I>::operator--() -> reverse_iterator&
{
  ++iter;
  return *this;
}

template<BidirectionalIterator I>
inline auto
reverse_iterator<I>::operator++(int) -> reverse_iterator
{
  reverse_iterator tmp = iter;
  --iter;
  return tmp;
}

template<BidirectionalIterator I>
inline auto
reverse_iterator<I>::operator--(int) -> reverse_iterator
{
  reverse_iterator tmp = iter;
  ++iter;
  return *this;
}

template<BidirectionalIterator I>
inline auto
reverse_iterator<I>::operator+=(difference_type n) -> reverse_iterator&
  requires RandomAccessIterator<I>()
{
  iter -= n;
  return *this;
}

template<BidirectionalIterator I>
inline auto
reverse_iterator<I>::operator+(difference_type n) -> reverse_iterator
  requires RandomAccessIterator<I>()
{
  return reverse_iterator(iter - n);
}

template<BidirectionalIterator I>
inline auto
reverse_iterator<I>::operator-=(difference_type n) -> reverse_iterator&
  requires RandomAccessIterator<I>()
{
  iter += n;
  return *this;
}

template<BidirectionalIterator I>
inline auto
reverse_iterator<I>::operator-(difference_type n) -> reverse_iterator
  requires RandomAccessIterator<I>()
{
  return reverse_iterator(iter + n);
}

template<BidirectionalIterator I>
inline auto
reverse_iterator<I>::operator[](difference_type n) -> reference
  requires RandomAccessIterator<I>()
{
  return *(iter + n);
}

// Equality

template<typename I1, typename I2>
  requires EqualityComparable<I1, I2>()
inline bool
operator==(reverse_iterator<I1> const& a, reverse_iterator<I2> const& b)
{
  return a.iter == b.iter;
}

template<typename I1, typename I2>
  requires EqualityComparable<I1, I2>()
inline bool
operator!=(reverse_iterator<I1> const& a, reverse_iterator<I2> const& b)
{
  return a.iter != b.iter;
}

// Ordering

template<typename I1, typename I2>
  requires TotallyOrdered<I1, I2>()
inline bool
operator<(reverse_iterator<I1> const& a, reverse_iterator<I2> const& b)
{
  return a.iter < b.iter;
}

template<typename I1, typename I2>
  requires TotallyOrdered<I1, I2>()
inline bool
operator>(reverse_iterator<I1> const& a, reverse_iterator<I2> const& b)
{
  return a.iter > b.iter;
}

template<typename I1, typename I2>
  requires TotallyOrdered<I1, I2>()
inline bool
operator<=(reverse_iterator<I1> const& a, reverse_iterator<I2> const& b)
{
  return a.iter <= b.iter;
}

template<typename I1, typename I2>
  requires TotallyOrdered<I1, I2>()
inline bool
operator>=(reverse_iterator<I1> const& a, reverse_iterator<I2> const& b)
{
  return a.iter >= b.iter;
}

// Arithmetic

// NOTE: Not to spec.
template<RandomAccessIterator I>
inline difference_type_t<I>
operator-(reverse_iterator<I> const& a, reverse_iterator<I> const& b)
{
  return b.iter - a.iter;
}

template<RandomAccessIterator I>
inline reverse_iterator<I>
operator+(difference_type_t<I> n, reverse_iterator<I> i)
{
  return reverse_iterator<I>(i.iter + n);
}

template<BidirectionalIterator I>
inline reverse_iterator<I>
make_reverse_iterator(I const& iter)
{
  return reverse_iterator<I>(iter);
}


// Insertion concepts
//
// NOTE: T is generally expected to be either X const& or X&&.

template<typename T>
struct iterator;

template<typename T>
struct iterator<T[]>
{
  using type = T*;
};

template<typename T, std::size_t N>
struct iterator<T[N]>
{
  using type = T*;
};

template<typename T>
  requires requires { T::iterator; }
struct iterator<T[]>
{
  using type = typename T::iterator;
};

template<typename T>
using iterator_t = typename iterator<T>::type;


template<typename C, typename T>
concept bool BackInsertable()
{
  return requires (C& c, T&& t) {
    c.push_back(std::forward<T>(t));
  };
}

template<typename C>
concept bool BackCopyInsertable()
{
  return BackInsertable<C, value_type_t<C> const&>();
}

template<typename C>
concept bool BackMoveInsertable()
{
  return BackInsertable<C, value_type_t<C>&&>();
}


template<typename C, typename T>
concept bool FrontInsertable()
{
  return requires (C& c, T&& t) {
    c.push_front(std::forward<T>(t));
  };
}

template<typename C>
concept bool FrontCopyInsertable()
{
  return FrontInsertable<C, value_type_t<C> const&>();
}

template<typename C>
concept bool FrontMoveInsertable()
{
  return FrontInsertable<C, value_type_t<C>&&>();
}

template<typename C, typename T>
concept bool Insertable()
{
  return requires (C& c, iterator_t<C> i, T&& t) {
    c.insert(i, std::forward<T>(t));
  };
}

template<typename C>
concept bool CopyInsertable()
{
  return Insertable<C, value_type_t<C> const&>();
}

template<typename C>
concept bool MoveInsertable()
{
  return Insertable<C, value_type_t<C>&&>();
}


// Back insert iterator

// FIXME: Requires something like back-insertion-sequence.
template<typename C>
struct back_insert_iterator
{
  using containe_type = C;
  using difference_type = difference_type_t<C>;

  constexpr back_insert_iterator()
    : cont(nullptr)
  { }

  explicit back_insert_iterator(C& c)
    : cont(&c)
  { }

  back_insert_iterator& operator=(value_type_t<C> const&) requires BackCopyInsertable<C>();
  back_insert_iterator& operator=(value_type_t<C>&&) requires BackMoveInsertable<C>();

  back_insert_iterator& operator*() { return *this; }

  back_insert_iterator& operator++()    { return *this; }
  back_insert_iterator  operator++(int) { return *this; }

  C* cont;
};

template<typename C>
inline auto
back_insert_iterator<C>::operator=(value_type_t<C> const& x) -> back_insert_iterator&
  requires BackCopyInsertable<C>()
{
  cont->push_back(x);
}

template<typename C>
inline auto
back_insert_iterator<C>::operator=(value_type_t<C>&& x) -> back_insert_iterator&
  requires BackMoveInsertable<C>()
{
  cont->push_back(std::move(x));
}

template<typename C>
inline back_insert_iterator<C>
back_inserter(C& c)
{
  return back_insert_iterator<C>(c);
}


// Front insert iterator

// FIXME: Requires something like front-insertion-sequence.
template<typename C>
struct front_insert_iterator
{
  using containe_type = C;
  using difference_type = difference_type_t<C>;

  constexpr front_insert_iterator()
    : cont(nullptr)
  { }

  explicit front_insert_iterator(C& c)
    : cont(&c)
  { }

  front_insert_iterator& operator=(value_type_t<C> const&) requires FrontCopyInsertable<C>();
  front_insert_iterator& operator=(value_type_t<C>&&) requires FrontMoveInsertable<C>();

  front_insert_iterator& operator*() { return *this; }

  front_insert_iterator& operator++()    { return *this; }
  front_insert_iterator  operator++(int) { return *this; }

  C* cont;
};

template<typename C>
inline auto
front_insert_iterator<C>::operator=(value_type_t<C> const& x) -> front_insert_iterator&
  requires FrontCopyInsertable<C>()
{
  cont->push_front(x);
}

template<typename C>
inline auto
front_insert_iterator<C>::operator=(value_type_t<C>&& x) -> front_insert_iterator&
  requires FrontMoveInsertable<C>()
{
  cont->push_front(std::move(x));
}

template<typename C>
inline front_insert_iterator<C>
front_inserter(C& c)
{
  return front_insert_iterator<C>(c);
}


// Move iterator

template<InputIterator I>
struct move_iterator
{
  using iterator_type     = I;
  using value_type        = value_type_t<I>;
  using reference         = remove_reference_t<reference_t<I>>;
  using difference_type   = difference_type_t<I>;
  using iterator_category = iterator_category_t<I>;

  explicit move_iterator(I i)
    : iter(i)
  { }

  template<ConvertibleTo<I> J>
  move_iterator(move_iterator<J> const&);

  template<ConvertibleTo<I> J>
  move_iterator& operator=(move_iterator<J> const&);

  I base() const { return iter; }

  reference operator*() const;

  move_iterator& operator++();
  move_iterator& operator--() requires BidirectionalIterator<I>();
  move_iterator operator++(int);
  move_iterator operator--(int) requires BidirectionalIterator<I>();

  move_iterator& operator+=(difference_type) requires RandomAccessIterator<I>();
  move_iterator& operator-=(difference_type) requires RandomAccessIterator<I>();

  reference operator[](difference_type n) const requires RandomAccessIterator<I>();

  I iter;
};

template<InputIterator I>
template<ConvertibleTo<I> J>
inline
move_iterator<I>::move_iterator(move_iterator<J> const& i)
  : iter(i)
{ }

template<InputIterator I>
template<ConvertibleTo<I> J>
inline auto
move_iterator<I>::operator=(move_iterator<J> const& i) -> move_iterator&
{
  iter = i.iter;
  return *this;
}

template<InputIterator I>
inline auto
move_iterator<I>::operator*() const -> reference
{
  return static_cast<reference>(*iter);
}

template<InputIterator I>
inline auto
move_iterator<I>::operator++() -> move_iterator&
{
  ++iter;
  return *this;
}

template<InputIterator I>
inline auto
move_iterator<I>::operator--() -> move_iterator&
  requires BidirectionalIterator<I>()
{
  --iter;
  return *this;
}

template<InputIterator I>
inline auto
move_iterator<I>::operator++(int) -> move_iterator
{
  move_iterator tmp = *this;
  ++iter;
  return tmp;
}

template<InputIterator I>
inline auto
move_iterator<I>::operator--(int) -> move_iterator
  requires BidirectionalIterator<I>()
{
  move_iterator tmp = *this;
  --iter;
  return tmp;
}

template<InputIterator I>
inline auto
move_iterator<I>::operator+=(difference_type n) -> move_iterator&
  requires RandomAccessIterator<I>()
{
  iter += n;
  return *this;
}

template<InputIterator I>
inline auto
move_iterator<I>::operator-=(difference_type n) -> move_iterator&
  requires RandomAccessIterator<I>()
{
  iter -= n;
  return *this;
}

template<InputIterator I>
inline auto
move_iterator<I>::operator[](difference_type n) const -> reference
  requires RandomAccessIterator<I>()
{
  return *(iter + n);
}


// Random access arithmetic

template<RandomAccessIterator I>
inline move_iterator<I>
operator+(move_iterator<I> const& i, difference_type_t<I> n)
{
  move_iterator<I> tmp = i;
  return tmp += n;
}

template<RandomAccessIterator I>
inline move_iterator<I>
operator+(difference_type_t<I> n, move_iterator<I> const& i)
{
  return i + n;
}

template<RandomAccessIterator I>
inline move_iterator<I>
operator-(move_iterator<I> const& i, difference_type_t<I> n)
{
  move_iterator<I> tmp = i;
  return tmp -= n;
}







} // namespace stl

#endif
