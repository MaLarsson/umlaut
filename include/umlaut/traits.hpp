#pragma once

#include <type_traits>

namespace umlaut {

template <typename T>
struct is_empty_base_optimizable : std::conjunction<
    std::is_empty<T>,
    std::negation<std::is_final<T>>
> {};

template <typename T>
inline constexpr bool is_empty_base_optimizable_v = is_empty_base_optimizable<T>::value;

} // namespace umlaut
