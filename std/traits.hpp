
#ifndef STL_TRAITS_HPP
#define STL_TRAITS_HPP

#include <cstddef>
#include <type_traits>
#include <utility>

namespace stl
{

template<bool B>
using boolean_constant = std::integral_constant<bool, B>;


template<typename T>
concept bool is_void_v = std::is_void<T>::value;


template<typename T>
constexpr bool is_integral_v = std::is_integral<T>::value;


template<typename T>
constexpr bool is_member_pointer_v = std::is_member_pointer<T>::value;


template<typename T>
constexpr bool is_lvalue_reference_v = std::is_lvalue_reference<T>::value;


template<typename T>
constexpr bool is_rvalue_reference_v = std::is_rvalue_reference<T>::value;


template<typename T>
constexpr bool is_reference_v = std::is_reference<T>::value;


template<typename T>
constexpr bool is_object_v = std::is_object<T>::value;


template<typename T>
constexpr bool is_variable_v = is_reference_v<T> || is_object_v<T>;


template<typename T>
constexpr bool is_signed_v = std::is_signed<T>::value;


template<typename T>
constexpr bool is_unsigned_v = std::is_unsigned<T>::value;


template<typename T>
constexpr bool is_destructible_v = std::is_destructible<T>::value;


template<typename T>
constexpr bool is_nothrow_move_constructible_v = std::is_nothrow_move_constructible<T>::value;


template<typename T>
constexpr bool is_nothrow_move_assignable_v = std::is_nothrow_move_assignable<T>::value;


template<typename T, typename U>
constexpr bool is_convertible_v = std::is_convertible<T, U>::value;


template<typename T>
using remove_reference_t = typename std::remove_reference<T>::type;


template<typename T>
using remove_cv_t = typename std::remove_cv<T>::type;


template<typename T>
using decay_t = typename std::decay<T>::type;


template<typename T>
using result_of_t = typename std::result_of<T>::type;


template<typename T, typename U>
using common_type_t = typename std::common_type<T, U>::type;


} // namespace stl

#endif
