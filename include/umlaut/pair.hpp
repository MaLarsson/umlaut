#pragma once

#include "traits.hpp"

#include <utility>
#include <cstddef>

namespace umlaut::detail {

template <typename T, std::size_t Index, bool = is_ebo_eligable_v<T>>
struct ebo_elem {
    using value_type = T;

    constexpr ebo_elem() : m_value() {}

    constexpr ebo_elem(T&& value)
	: m_value(std::forward<T>(value)) {}

 private:
    value_type m_value;
};

template <typename T, std::size_t Index>
struct ebo_elem<T, Index, true> : T {
    using value_type = T;

    constexpr ebo_elem() : T() {}

    constexpr ebo_elem(T&& value)
	: value_type(std::forward<T>(value)) {}
};

} // namespace umlaut::detail

namespace umlaut {

template <typename T, typename U>
class pair : private detail::ebo_elem<T, 0>, private detail::ebo_elem<U, 1> {
    using Base1 = detail::ebo_elem<T, 0>;
    using Base2 = detail::ebo_elem<U, 1>;

 public:
    using first_type = typename Base1::value_type;
    using second_type = typename Base2::value_type;

    constexpr pair() : Base1(), Base2() {}

    constexpr pair(T&& first, U&& second)
	: Base1(std::forward<T>(first)), Base2(std::forward<U>(second)) {}
};

} // namespace umlaut
