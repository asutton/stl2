
#include <std/concepts.hpp>

#include <string>
#include <memory>

static_assert(stl::SameAs<int, int>());
static_assert(!stl::SameAs<int, int const>());

struct B { };
struct D : B { };

static_assert(stl::DerivedFrom<B, B>());
static_assert(stl::DerivedFrom<D, B>());
static_assert(!stl::DerivedFrom<B, D>());


// TODO: Add more of these.
static_assert(stl::ConvertibleTo<int, int>());
static_assert(stl::ConvertibleTo<int const, int>());
static_assert(stl::ConvertibleTo<int&, int>());
static_assert(stl::ConvertibleTo<int const&, int>());


template<typename T, typename U>
  requires stl::Common<T, U>()
constexpr bool test_common() { return true; }

static_assert(test_common<int, int>());
static_assert(test_common<int, double>());


template<typename T>
  requires stl::Swappable<T>()
constexpr bool test_swappable() { return true; }

template<typename T, typename U>
  requires stl::Swappable<T, U>()
constexpr bool test_swappable() { return true; }

// TODO: Find a good negative case.
static_assert(test_swappable<int>());
static_assert(test_swappable<double>());

// TODO: Find cases where specializations allow swapping different types.
static_assert(test_swappable<int, int>());


template<typename T>
constexpr bool test_boolean() { return false; }

template<stl::Boolean T>
constexpr bool test_boolean() { return true; }

// TODO: Exercise this a little better
static_assert(test_boolean<bool>());
static_assert(!test_boolean<int>());


template<typename T>
constexpr bool test_equality_comparable() { return false; }

template<stl::EqualityComparable T>
constexpr bool test_equality_comparable() { return true; }

static_assert(test_equality_comparable<int>());
static_assert(test_equality_comparable<double>());
static_assert(test_equality_comparable<std::string>());


template<typename T>
constexpr bool test_totally_ordered() { return false; }

template<stl::TotallyOrdered T>
constexpr bool test_totally_ordered() { return true; }

static_assert(test_totally_ordered<int>());
static_assert(test_totally_ordered<double>());
static_assert(test_totally_ordered<std::string>());


template<typename T>
constexpr bool test_variable() { return false; }

template<stl::Variable T>
constexpr bool test_variable() { return true; }

static_assert(test_variable<int>());
static_assert(test_variable<int&>());
static_assert(!test_variable<int()>());


template<typename T, typename... Args>
constexpr bool test_initializable() { return false; }

template<stl::Variable T, typename... Args>
  requires stl::Initializable<T, Args...>()
constexpr bool test_initializable() { return true; }

static_assert(stl::Initializable<int, int>());
static_assert(stl::Initializable<int, int const>());
static_assert(stl::Initializable<int, int&>());
static_assert(stl::Initializable<int, int const&>());

// TODO: Add initialization checks for rvalue references
static_assert(stl::Initializable<int&, int&>());
static_assert(stl::Initializable<int const&, int>());
static_assert(stl::Initializable<int const&, int&>());
static_assert(stl::Initializable<int const&, int const&>());


template<typename T>
constexpr bool test_object() { return false; }

template<stl::Object T>
constexpr bool test_object() { return true; }

static_assert(test_object<int>());
static_assert(!test_object<int&>());
static_assert(!test_object<int()>());


template<typename T>
constexpr bool test_dstructible() { return false; }

template<stl::Destructible T>
constexpr bool test_dstructible() { return true; }

// TODO: More tests.
static_assert(test_dstructible<int>());


// Constructible

template<typename T, typename... Args>
constexpr bool test_constructible() { return false; }

template<typename T, typename... Args>
  requires stl::Constructible<T, Args...>()
constexpr bool test_constructible() { return true; }

// TODOD: More tests!
static_assert(test_constructible<int, int>());
static_assert(test_constructible<int const, int>());


// Move constructible

template<typename T>
constexpr bool test_move_constructible() { return false; }

template<stl::MoveConstructible T>
constexpr bool test_move_constructible() { return true; }

// TODO: More tests!
static_assert(test_move_constructible<int>());
static_assert(test_move_constructible<int const>()); // FIXME: Seems wrong
static_assert(!test_move_constructible<int&>());
static_assert(test_move_constructible<std::string>());
static_assert(test_move_constructible<std::unique_ptr<int>>());


// Move assignable

template<typename T>
constexpr bool test_move_assignable() { return false; }

template<stl::MoveConstructible T>
constexpr bool test_move_assignable() { return true; }

// TODO: More tests!
static_assert(test_move_assignable<int>());
static_assert(test_move_assignable<int const>()); // FIXME: Seems wrong
static_assert(!test_move_assignable<int&>());
static_assert(test_move_assignable<std::string>());
static_assert(test_move_assignable<std::unique_ptr<int>>());


// Copy constructible

template<typename T>
constexpr bool test_copy_constructible() { return false; }

template<stl::CopyConstructible T>
constexpr bool test_copy_constructible() { return true; }

// TODO: More tests!
static_assert(test_copy_constructible<int>());
static_assert(test_copy_constructible<int const>());
static_assert(!test_copy_constructible<int&>());
static_assert(test_copy_constructible<std::string>());
static_assert(!test_copy_constructible<std::unique_ptr<int>>());


// Copy assignable

template<typename T>
constexpr bool test_copy_assignable() { return false; }

template<stl::CopyAssignable T>
constexpr bool test_copy_assignable() { return true; }

// TODO: More tests!
static_assert(test_copy_assignable<int>());
static_assert(!test_copy_assignable<int const>());
static_assert(!test_copy_assignable<int&>());
static_assert(test_copy_assignable<std::string>());
static_assert(!test_copy_assignable<std::unique_ptr<int>>());


int main()
{
}
