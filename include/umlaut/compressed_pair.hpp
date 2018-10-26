// Defines ul::compressed_pair.
//
// Copyright Marcus Larsson 2018
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#pragma once

#include "traits.hpp"

#include <utility>
#include <cstddef>

namespace ul {
namespace detail {

template <typename T, std::size_t Index, bool = is_empty_base_optimizable_v<T>>
struct compressed_element {
    using value_type = T;

    constexpr compressed_element() : m_value() {}

    template <typename U = T>
    constexpr compressed_element(U&& value) : m_value(std::forward<U>(value)) {}

    constexpr value_type& get_value() & { return m_value; }
    constexpr value_type&& get_value() && { return std::move(m_value); }
    constexpr const value_type& get_value() const & { return m_value; }
    constexpr const value_type&& get_value() const && { return std::move(m_value); }

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

    constexpr value_type& get_value() & { return *this; }
    constexpr value_type&& get_value() && { return std::move(*this); }
    constexpr const value_type& get_value() const & { return *this; }
    constexpr const value_type&& get_value() const && { return std::move(*this); }
};

} // namespace detail

template <typename First, typename Second>
class compressed_pair : private detail::compressed_element<First, 0>,
			private detail::compressed_element<Second, 1> {
    using first_base = detail::compressed_element<First, 0>;
    using second_base = detail::compressed_element<Second, 1>;

 public:
    using first_type = typename first_base::value_type;
    using second_type = typename second_base::value_type;

    constexpr compressed_pair() : first_base(), second_base() {}

    template <typename T = First, typename U = Second>
    constexpr compressed_pair(T&& first, U&& second)
	: first_base(std::forward<T>(first)), second_base(std::forward<U>(second)) {}

    constexpr first_type& first() & {
	return static_cast<first_base&>(*this).get_value();
    }

    constexpr first_type&& first() && {
	return std::move(static_cast<first_base&&>(*this).get_value());
    }

    constexpr const first_type& first() const & {
	return static_cast<const first_base&>(*this).get_value();
    }

    constexpr const first_type&& first() const && {
	return std::move(static_cast<const first_base&&>(*this).get_value());
    }

    constexpr second_type& second() & {
	return static_cast<second_base&>(*this).get_value();
    }

    constexpr second_type&& second() && {
	return std::move(static_cast<second_base&&>(*this).get_value());
    }

    constexpr const second_type& second() const & {
	return static_cast<const second_base&>(*this).get_value();
    }

    constexpr const second_type&& second() const && {
	return std::move(static_cast<const second_base&&>(*this).get_value());
    }
};

} // namespace ul
