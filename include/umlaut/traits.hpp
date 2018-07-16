/// @file
/// Defines common type traits.
///
/// @copyright Marcus Larsson 2018
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#pragma once

#include <type_traits>


namespace umlaut::detail {

template <std::size_t I, typename T>
struct indexed { using type = T; };

template <typename Is, typename ...Ts>
struct indexer;

template <std::size_t ...Is, typename ...Ts>
struct indexer<std::index_sequence<Is...>, Ts...> : indexed<Is, Ts>... {};

template <std::size_t I, typename T>
static indexed<I, T> select(indexed<I, T>);

} // namespace umlaut::detail

namespace umlaut {

/// @brief Traits class for parameter pack indexing.
template <std::size_t I, typename ...Ts>
using nth_element = typename decltype(detail::select<I>(
    detail::indexer<std::index_sequence_for<Ts...>, Ts...>{}
))::type;

/// @brief Traits class used to check whether type T can be "optimized away" using EBO.
template <typename T>
struct is_empty_base_optimizable : std::conjunction<
    std::is_empty<T>,
    std::negation<std::is_final<T>>
> {};

/// @relates is_empty_base_optimizable
template <typename T>
inline constexpr bool is_empty_base_optimizable_v = is_empty_base_optimizable<T>::value;

} // namespace umlaut
