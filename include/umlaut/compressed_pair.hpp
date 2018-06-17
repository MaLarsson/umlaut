#pragma once

#include "traits.hpp"

#include <utility>
#include <cstddef>

namespace umlaut::detail {

template <typename T, std::size_t Index, bool = is_empty_base_optimizable_v<T>>
struct compressed_element {
    using value_type = T;

    constexpr compressed_element() : m_value() {}

    constexpr compressed_element(T&& value)
	: m_value(std::forward<T>(value)) {}

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

    constexpr compressed_element() : T() {}

    constexpr compressed_element(T&& value)
	: value_type(std::forward<T>(value)) {}

    constexpr value_type& get_value() {
	return *this;
    }

    constexpr const value_type& get_value() const {
	return *this;
    }
};

} // namespace umlaut::detail

namespace umlaut {

template <typename T, typename U>
class compressed_pair : private detail::compressed_element<T, 0>,
			private detail::compressed_element<U, 1> {
    using Base1 = detail::compressed_element<T, 0>;
    using Base2 = detail::compressed_element<U, 1>;

 public:
    using first_type = typename Base1::value_type;
    using second_type = typename Base2::value_type;

    constexpr compressed_pair() : Base1(), Base2() {}

    constexpr compressed_pair(T&& first, U&& second)
	: Base1(std::forward<T>(first)), Base2(std::forward<U>(second)) {}

    constexpr first_type& first() {
	return static_cast<Base1&>(*this).get_value();
    }

    constexpr const first_type& first() const {
	return static_cast<const Base1&>(*this).get_value();
    }

    constexpr second_type& second() {
	return static_cast<Base2&>(*this).get_value();
    }

    constexpr const second_type& second() const {
	return static_cast<const Base2&>(*this).get_value();
    }
};

} // namespace umlaut
