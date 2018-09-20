/// @file
/// Defines common type traits.
///
/// @copyright Marcus Larsson 2018
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#pragma once

#include "config.hpp"

#include <type_traits>
#include <cstddef>

#if UMLAUT_HAS_BUILTIN(__type_pack_element)
#define UMLAUT_USE_TYPE_PACK_ELEMENT_INTRINSIC
#endif

namespace ul {
namespace detail {

template <std::size_t I, typename T>
struct indexed { using type = T; };

template <typename Is, typename ...Ts>
struct indexer;

template <std::size_t ...Is, typename ...Ts>
struct indexer<std::index_sequence<Is...>, Ts...> : indexed<Is, Ts>... {};

template <std::size_t I, typename T>
[[maybe_unused]] static indexed<I, T> select(indexed<I, T>);

} // namespace detail

/// @brief Traits class for parameter pack indexing.
template <std::size_t I, typename ...Ts>
#if defined(UMLAUT_USE_TYPE_PACK_ELEMENT_INTRINSIC)
using pack_element_t = __type_pack_element<I, Ts...>;
#else
using pack_element_t = typename decltype(detail::select<I>(
    detail::indexer<std::index_sequence_for<Ts...>, Ts...>{}
))::type;
#endif


/// @brief Traits class used to check whether type T can be "optimized away" using EBO.
template <typename T>
struct is_empty_base_optimizable : std::conjunction<
    std::is_empty<T>,
    std::negation<std::is_final<T>>
> {};

/// @relates is_empty_base_optimizable
template <typename T>
inline constexpr bool is_empty_base_optimizable_v = is_empty_base_optimizable<T>::value;


/// @brief Traits class used to remove cv-qualifiers and reference.
template <typename T>
struct remove_cvref { using type = std::remove_cv_t<std::remove_reference_t<T>>; };

/// @relates remove_cvref
template <typename T>
using remove_cvref_t = typename remove_cvref<T>::type;


/// @brief Traits class used to determine if an iterator is contigous or not.
template <typename T>
struct is_contiguous_iterator : std::is_pointer<T> {};

/// @relates is_contiguous_iterator
template <typename T>
inline constexpr bool is_contiguous_iterator_v = is_contiguous_iterator<T>::value;


/// @brief Traits class used a determine priority of f.e. overloads.
template <std::size_t N>
struct priority_tag : /** @cond */ priority_tag<N-1> /** @endcond */ {};

template <>
struct priority_tag<0> {};

namespace detail {

template <typename T>
auto itr_helper(priority_tag<1>) -> std::bool_constant<T::is_trivially_relocatable::value>;

template <typename T>
auto itr_helper(priority_tag<0>) -> std::bool_constant<
    std::is_trivially_move_constructible_v<T> &&
    std::is_trivially_destructible_v<T>
>;

} // namespace detail

/// @brief Traits class used to determine if a type T is trivially relocatable.
template <typename T>
struct is_trivially_relocatable : decltype(detail::itr_helper<T>(priority_tag<1>{})) {};

/// @relates is_trivially_relocatable
template <typename T>
inline constexpr bool is_trivially_relocatable_v = is_trivially_relocatable<T>::value;

} // namespace ul
