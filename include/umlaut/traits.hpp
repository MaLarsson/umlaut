/// @file
/// Defines common type traits.
///
/// @copyright Marcus Larsson 2018
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#pragma once

#include <type_traits>

#if defined(__has_builtin)
#if __has_builtin(__type_pack_element)
#define UMLAUT_USE_TYPE_PACK_ELEMENT_INTRINSIC
#endif
#endif

namespace umlaut::detail {

template <std::size_t I, typename T>
struct indexed { using type = T; };

template <typename Is, typename ...Ts>
struct indexer;

template <std::size_t ...Is, typename ...Ts>
struct indexer<std::index_sequence<Is...>, Ts...> : indexed<Is, Ts>... {};

template <std::size_t I, typename T>
[[maybe_unused]] static indexed<I, T> select(indexed<I, T>);

} // namespace umlaut::detail

namespace umlaut {

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

} // namespace umlaut
