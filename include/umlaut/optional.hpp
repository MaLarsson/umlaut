/// @file
/// Defines umlaut::optional.
///
/// @copyright Marcus Larsson 2018
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#pragma once

#include "traits.hpp"
#include "special_members.hpp"

#include <cstddef>
#include <type_traits>
#include <utility>
#include <memory>
#include <exception>
#include <functional>

namespace umlaut {

template <typename T>
class optional;

namespace detail {

template <typename T>
struct is_optional_impl : std::false_type {};

template <typename T>
struct is_optional_impl<optional<T>> : std::true_type {};

} // namespace detail

template <typename T>
using is_optional = detail::is_optional_impl<remove_cvref_t<T>>;

template <typename T>
inline constexpr bool is_optional_v = is_optional<T>::value;

struct nullopt_t {
    struct do_not_use {};
    constexpr explicit nullopt_t(do_not_use) noexcept {}
};

inline constexpr nullopt_t nullopt{nullopt_t::do_not_use{}};

class bad_optional_access : public std::exception {
 public:
    bad_optional_access() = default;
    const char* what() const noexcept override { return "Optional has no value"; }
};

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

    struct empty_byte {};
    union {
	empty_byte m_dummy;
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

    struct empty_byte {};
    union {
	empty_byte m_dummy;
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

template <typename T>
using optional_delete_ctor_base = delete_ctor_base<
    std::is_copy_constructible_v<T>,
    std::is_move_constructible_v<T>
>;

template <typename T>
using optional_delete_assign_base = delete_assign_base<
    std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T>,
    std::is_move_constructible_v<T> && std::is_move_assignable_v<T>
>;

} // namespace detail

template <typename T>
class optional : private detail::optional_common_base<T>,
		 private detail::optional_delete_ctor_base<T>,
		 private detail::optional_delete_assign_base<T> {
    using base = detail::optional_common_base<T>;

 public:
    using value_type = T;

    constexpr optional() noexcept = default;

    constexpr optional(nullopt_t) noexcept {}

    constexpr optional(const optional& rhs) = default;
    constexpr optional(optional&& rhs) = default;

    template <typename ...Args>
    constexpr optional(std::in_place_t, Args&&... args)
	: base(std::in_place, std::forward<Args>(args)...) {}

    optional& operator=(const optional& rhs) = default;
    optional& operator=(optional&& rhs) = default;

    constexpr value_type& operator*() & { return this->m_value; }
    constexpr value_type&& operator*() && { return std::move(this->m_value); }
    constexpr const value_type& operator*() const & { return this->m_value; }
    constexpr const value_type&& operator*() const && { return std::move(this->m_value); }

    constexpr value_type* operator->() { return &this->m_value; }
    constexpr const value_type* operator->() const { return &this->m_value; }

    constexpr explicit operator bool() const noexcept { return this->m_has_value; }

    constexpr bool has_value() const noexcept { return this->m_has_value; }

    constexpr value_type& value() & {
	if (has_value())
	    return this->m_value;

	throw bad_optional_access{};
    }

    constexpr value_type&& value() && {
	if (has_value())
	    return std::move(this->m_value);

	throw bad_optional_access{};
    }

    constexpr const value_type& value() const & {
	if (has_value())
	    return this->m_value;

	throw bad_optional_access{};
    }

    constexpr const value_type&& value() const && {
	if (has_value())
	    return std::move(this->m_value);

	throw bad_optional_access{};
    }

    template <typename F>
    constexpr auto then(F&& f) & {
	static_assert(std::is_invocable_v<F, T&>, "F must be invocable with T&");

	using result_t = std::invoke_result_t<F, T&>;

	if constexpr (is_optional_v<result_t>) {
	    if (has_value())
		return std::invoke(std::forward<F>(f), this->m_value);

	    return result_t(nullopt);
	}
	else {
	    if (has_value())
		return optional<result_t>(std::in_place, std::invoke(std::forward<F>(f), this->m_value));

	    return optional<result_t>(nullopt);
	}
    }

    template <typename F>
    constexpr auto then(F&& f) && {
	static_assert(std::is_invocable_v<F, T&&>, "F must be invocable with T&&");

	using result_t = std::invoke_result_t<F, T&&>;

	if constexpr (is_optional_v<result_t>) {
	    if (has_value())
		return std::invoke(std::forward<F>(f), std::move(this->m_value));

	    return result_t(nullopt);
	}
	else {
	    if (has_value())
		return optional<result_t>(std::in_place, std::invoke(std::forward<F>(f), std::move(this->m_value)));

	    return optional<result_t>(nullopt);
	}
    }

    template <typename F>
    constexpr auto then(F&& f) const & {
	static_assert(std::is_invocable_v<F, const T&>, "F must be invocable with const T&");

	using result_t = std::invoke_result_t<F, const T&>;

	if constexpr (is_optional_v<result_t>) {
	    if (has_value())
		return std::invoke(std::forward<F>(f), this->m_value);

	    return result_t(nullopt);
	}
	else {
	    if (has_value())
		return optional<result_t>(std::in_place, std::invoke(std::forward<F>(f), this->m_value));

	    return optional<result_t>(nullopt);
	}
    }

    template <typename F>
    constexpr auto then(F&& f) const && {
	static_assert(std::is_invocable_v<F, const T&>, "F must be invocable with const T&");

	using result_t = std::invoke_result_t<F, const T&>;

	if constexpr (is_optional_v<result_t>) {
	    if (has_value())
		return std::invoke(std::forward<F>(f), std::move(this->m_value));

	    return result_t(nullopt);
	}
	else {
	    if (has_value())
		return optional<result_t>(std::in_place, std::invoke(std::forward<F>(f), std::move(this->m_value)));

	    return optional<result_t>(nullopt);
	}
    }

    template <typename F>
    constexpr auto catch_error(F&& f) & {
	static_assert(std::is_invocable_v<F>, "F must be invocable");

	using result_t = std::invoke_result_t<F>;

	if (has_value())
	    return *this;

	if constexpr (is_optional_v<result_t>) {
	    return std::invoke(std::forward<F>(f));
	}
	else if constexpr (std::is_void_v<result_t>) {
	    std::invoke(std::forward<F>(f));
	    return *this;
	}
	else {
	    return optional<result_t>(std::in_place, std::invoke(std::forward<F>(f)));
	}
    }

    template <typename F>
    constexpr auto catch_error(F&& f) && {
	static_assert(std::is_invocable_v<F>, "F must be invocable");

	using result_t = std::invoke_result_t<F>;

	if (has_value())
	    return std::move(*this);

	if constexpr (is_optional_v<result_t>) {
	    return std::invoke(std::forward<F>(f));
	}
	else if constexpr (std::is_void_v<result_t>) {
	    std::invoke(std::forward<F>(f));
	    return std::move(*this);
	}
	else {
	    return optional<result_t>(std::in_place, std::invoke(std::forward<F>(f)));
	}
    }

    template <typename F>
    constexpr auto catch_error(F&& f) const & {
	static_assert(std::is_invocable_v<F>, "F must be invocable");

	using result_t = std::invoke_result_t<F>;

	if (has_value())
	    return *this;

	if constexpr (is_optional_v<result_t>) {
	    return std::invoke(std::forward<F>(f));
	}
	else if constexpr (std::is_void_v<result_t>) {
	    std::invoke(std::forward<F>(f));
	    return *this;
	}
	else {
	    return optional<result_t>(std::in_place, std::invoke(std::forward<F>(f)));
	}
    }

    template <typename F>
    constexpr auto catch_error(F&& f) const && {
	static_assert(std::is_invocable_v<F>, "F must be invocable");

	using result_t = std::invoke_result_t<F>;

	if (has_value())
	    return std::move(*this);

	if constexpr (is_optional_v<result_t>) {
	    return std::invoke(std::forward<F>(f));
	}
	else if constexpr (std::is_void_v<result_t>) {
	    std::invoke(std::forward<F>(f));
	    return std::move(*this);
	}
	else {
	    return optional<result_t>(std::in_place, std::invoke(std::forward<F>(f)));
	}
    }
};

} // namespace umlaut
