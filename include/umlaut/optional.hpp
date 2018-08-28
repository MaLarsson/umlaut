/// @file
/// Defines umlaut::optional.
///
/// @copyright Marcus Larsson 2018
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#pragma once

#include "special_members.hpp"

#include <cstddef>
#include <type_traits>
#include <utility>
#include <memory>
#include <exception>
#include <functional>

namespace umlaut {

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




// temporary invoke_result_t
template <typename F, typename, typename... Args> struct invoke_result_impl;

template <typename F, typename... Args>
struct invoke_result_impl<
    F, decltype(std::invoke(std::declval<F>(), std::declval<Args>()...), void()),
    Args...> {
    using type = decltype(std::invoke(std::declval<F>(), std::declval<Args>()...));
};

template <typename F, typename... Args>
using invoke_result = invoke_result_impl<F, void, Args...>;

template <typename F, typename... Args>
using invoke_result_t = typename invoke_result<F, Args...>::type;




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

    optional& operator=(const optional& rhs) = default;
    optional& operator=(optional&& rhs) = default;

    template <typename ...Args>
    constexpr optional(std::in_place_t, Args&&... args)
	: base(std::in_place, std::forward<Args>(args)...) {}

    constexpr bool has_value() const noexcept { return this->m_has_value; }

    constexpr explicit operator bool() const noexcept { return this->m_has_value; }

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
	using result = detail::invoke_result_t<F, T&>;

	if (has_value())
	    return std::invoke(std::forward<F>(f), this->m_value);

	return result(nullopt);
    }

    template <typename F>
    constexpr auto then(F&& f) && {
	using result = detail::invoke_result_t<F, T&&>;

	if (has_value())
	    return std::invoke(std::forward<F>(f), std::move(this->m_value));

	return result(nullopt);
    }

    template <typename F>
    constexpr auto then(F&& f) const & {
	using result = detail::invoke_result_t<F, const T&>;

	if (has_value())
	    return std::invoke(std::forward<F>(f), this->m_value);

	return result(nullopt);
    }

    template <typename F>
    constexpr auto then(F&& f) const && {
	using result = detail::invoke_result_t<F, const T&>;

	if (has_value())
	    return std::invoke(std::forward<F>(f), std::move(this->m_value));

	return result(nullopt);
    }
};

} // namespace umlaut
