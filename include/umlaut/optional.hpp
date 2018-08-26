/// @file
/// Defines umlaut::optional.
///
/// @copyright Marcus Larsson 2018
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#pragma once

#include <cstddef>
#include <type_traits>
#include <utility>
#include <memory>
#include <optional>

namespace umlaut {
namespace detail {

template <typename T, bool = std::is_trivially_destructible_v<T>>
struct optional_maybe_dtor {
    constexpr optional_maybe_dtor() noexcept
	: m_dummy(), m_has_value(false) {}

    template <typename ...Args>
    constexpr optional_maybe_dtor(std::in_place_t, Args&&... args)
	: m_value(std::forward<Args>(args)...), m_has_value(true) {}

    ~optional_maybe_dtor() {
	if (m_has_value) {
	    m_value.~T();
	    m_has_value = false;
	}
    }

    union {
	std::byte m_dummy;
	T m_value;
    };

    bool m_has_value;
};

template <typename T>
struct optional_maybe_dtor<T, true> {
    constexpr optional_maybe_dtor() noexcept
	: m_dummy(), m_has_value(false) {}

    template <typename ...Args>
    constexpr optional_maybe_dtor(std::in_place_t, Args&&... args)
	: m_value(std::forward<Args>(args)...), m_has_value(true) {}

    union {
	std::byte m_dummy;
	T m_value;
    };

    bool m_has_value;
};

template <typename T>
struct optional_common_base : optional_maybe_dtor<T> {
    using optional_maybe_dtor<T>::optional_maybe_dtor;

    template <typename ...Args>
    void construct(Args&&... args) {
	::new (std::addressof(this->m_value)) T(std::forward<Args>(args)...);
	this->m_has_value = true;
    }

    constexpr bool has_value() const noexcept { return this->m_has_value; }
};

} // namespace detail

template <typename T>
class optional : private detail::optional_common_base<T> {
    using base = detail::optional_common_base<T>;

 public:
    using value_type = T;

    constexpr optional() noexcept = default;

    template <typename ...Args>
    constexpr optional(std::in_place_t, Args&&... args)
	: base(std::in_place, std::forward<Args>(args)...) {}

    constexpr bool has_value() const noexcept { return this->m_has_value; }

    constexpr explicit operator bool() const noexcept { return this->m_has_value; }

    constexpr value_type& value() & {
	if (has_value())
	    return this->m_value;

	throw std::bad_optional_access{};
    }

    constexpr value_type& value() && {
	if (has_value())
	    return std::move(this->m_value);

	throw std::bad_optional_access{};
    }

    constexpr const value_type& value() const & {
	if (has_value())
	    return this->m_value;

	throw std::bad_optional_access{};
    }

    constexpr const value_type& value() const && {
	if (has_value())
	    return std::move(this->m_value);

	throw std::bad_optional_access{};
    }
};

} // namespace umlaut
