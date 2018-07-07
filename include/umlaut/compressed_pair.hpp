/// @file
/// Defines umlaut::compressed_pair.
///
/// @copyright Marcus Larsson 2018
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#pragma once

#include "traits.hpp"

#include <utility>
#include <cstddef>

namespace umlaut::detail {

template <typename T, std::size_t Index, bool = is_empty_base_optimizable_v<T>>
struct compressed_element {
    using value_type = T;

    constexpr compressed_element() : m_value() {}

    template <typename U = T>
    constexpr compressed_element(U&& value)
	: m_value(std::forward<U>(value)) {}

    constexpr value_type& get_value() {
	return m_value;
    }

    constexpr const value_type& get_value() const {
	return m_value;
    }

 private:
    value_type m_value;
};

template <typename T, std::size_t Index>
struct compressed_element<T, Index, true> : T {
    using value_type = T;

    constexpr compressed_element() : value_type() {}

    template <typename U = T>
    constexpr compressed_element(U&& value)
	: value_type(std::forward<U>(value)) {}

    constexpr value_type& get_value() {
	return *this;
    }

    constexpr const value_type& get_value() const {
	return *this;
    }
};

} // namespace umlaut::detail

namespace umlaut {

/// Generic container for two elements.
template <typename First, typename Second>
class compressed_pair : private detail::compressed_element<First, 0>,
			private detail::compressed_element<Second, 1> {
    using Base1 = detail::compressed_element<First, 0>;
    using Base2 = detail::compressed_element<Second, 1>;

 public:
    /// @name Aliases
    /// @{
    using first_type = First;
    using second_type = Second;
    // @}

    /// @brief Default constructs the `compressed_pair`.
    constexpr compressed_pair() : Base1(), Base2() {}

    /// @brief TODO ...
    template <typename T = First, typename U = Second>
    constexpr compressed_pair(T&& first, U&& second)
	: Base1(std::forward<T>(first)),
	  Base2(std::forward<U>(second)) {}

    /// @brief TODO ...
    constexpr first_type& first() {
	return static_cast<Base1&>(*this).get_value();
    }

    /// @brief Const overload of `compressed_pair::first()`.
    constexpr const first_type& first() const {
	return static_cast<const Base1&>(*this).get_value();
    }

    /// @brief TODO ...
    constexpr second_type& second() {
	return static_cast<Base2&>(*this).get_value();
    }

    /// @brief Const overload of `compressed_pair::second()`.
    constexpr const second_type& second() const {
	return static_cast<const Base2&>(*this).get_value();
    }
};

} // namespace umlaut
