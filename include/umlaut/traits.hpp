#pragma once

#include <type_traits>

namespace umlaut {

template <typename T>
struct is_ebo_eligable : std::conjunction<std::is_empty<T>, std::negation<std::is_final<T>>> {};

template <typename T>
inline constexpr bool is_ebo_eligable_v = is_ebo_eligable<T>::value;

} // namespace umlaut
