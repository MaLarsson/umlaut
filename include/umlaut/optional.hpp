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

template <typename T, typename U>
using optional_enable_converting_constructors_t = std::enable_if_t<
    !std::is_constructible_v<T, optional<U>&> &&
    !std::is_constructible_v<T, const optional<U>&> &&
    !std::is_constructible_v<T, optional<U>&&> &&
    !std::is_constructible_v<T, const optional<U>&&> &&
    !std::is_convertible_v<optional<U>&, T> &&
    !std::is_convertible_v<const optional<U>&, T> &&
    !std::is_convertible_v<optional<U>&&, T> &&
    !std::is_convertible_v<const optional<U>&&, T>
>;

template <typename T, typename U>
using optional_enable_converting_assignment_t = std::enable_if_t<
    !std::is_constructible_v<T, optional<U>&> &&
    !std::is_constructible_v<T, const optional<U>&> &&
    !std::is_constructible_v<T, optional<U>&&> &&
    !std::is_constructible_v<T, const optional<U>&&> &&
    !std::is_convertible_v<optional<U>&, T> &&
    !std::is_convertible_v<const optional<U>&, T> &&
    !std::is_convertible_v<optional<U>&&, T> &&
    !std::is_convertible_v<const optional<U>&&, T> &&
    !std::is_assignable_v<T&, optional<U>&> &&
    !std::is_assignable_v<T&, const optional<U>&> &&
    !std::is_assignable_v<T&, optional<U>&&> &&
    !std::is_assignable_v<T&, const optional<U>&&>
>;

template <typename T, typename U>
using optional_enable_forward_assignment_t = std::enable_if_t<
    !std::is_same_v<remove_cvref<U>, optional<T>> &&
    std::is_constructible_v<T, U> &&
    std::is_assignable_v<T&, U> &&
    !std::conjunction_v<std::is_scalar<T>, std::is_same<T, std::decay_t<U>>>
>;

template <typename T, typename U>
using enable_forward_value_t = std::enable_if_t<
    !std::is_same_v<remove_cvref_t<U>, std::in_place_t> &&
    !std::is_same_v<remove_cvref_t<U>, optional<T>>
>;

template <typename T, bool = std::is_trivially_destructible_v<T>>
struct optional_maybe_dtor {
    using value_type = T;

    constexpr optional_maybe_dtor() noexcept
	: m_dummy(), m_has_value(false) {}

    template <typename ...Args>
    constexpr explicit optional_maybe_dtor(std::in_place_t, Args&&... args)
	: m_value(std::forward<Args>(args)...), m_has_value(true) {}

    ~optional_maybe_dtor() {
	if (m_has_value)
	    m_value.~value_type();
    }

    void destroy() noexcept {
	if (m_has_value) {
	    m_value.~value_type();
	    m_has_value = false;
	}
    }

    struct empty_byte {};
    union {
	empty_byte m_dummy;
	value_type m_value;
    };

    bool m_has_value;
};

template <typename T>
struct optional_maybe_dtor<T, true> {
    using value_type = T;

    constexpr optional_maybe_dtor() noexcept
	: m_dummy(), m_has_value(false) {}

    template <typename ...Args>
    constexpr explicit optional_maybe_dtor(std::in_place_t, Args&&... args)
	: m_value(std::forward<Args>(args)...), m_has_value(true) {}

    void destroy() noexcept {
	if (m_has_value)
	    m_has_value = false;
    }

    struct empty_byte {};
    union {
	empty_byte m_dummy;
	value_type m_value;
    };

    bool m_has_value;
};

template <typename T>
struct optional_storage_base : optional_maybe_dtor<T> {
    using value_type = T;
    using optional_maybe_dtor<T>::optional_maybe_dtor;

    template <typename ...Args>
    void construct(Args&&... args) {
	::new (std::addressof(this->m_value)) value_type(std::forward<Args>(args)...);
	this->m_has_value = true;
    }

    constexpr bool has_value() const noexcept { return this->m_has_value; }

    template <typename U>
    void construct_from(U&& other) {
	if (other.has_value())
	    construct(*std::forward<U>(other));
    }

    template <typename U>
    void assign_from(U&& other) {
	if (has_value() == other.has_value()) {
	    if (has_value())
		this->m_value = *std::forward<U>(other);
	}
	else {
	    if (has_value())
		this->destroy();
	    else
		construct(*std::forward<U>(other));
	}
    }
};

template <typename T, bool = std::is_trivially_copy_constructible_v<T>>
struct optional_copy_base : optional_storage_base<T> {
    using optional_storage_base<T>::optional_storage_base;
};

template <typename T>
struct optional_copy_base<T, false> : optional_storage_base<T> {
    using optional_storage_base<T>::optional_storage_base;

    optional_copy_base() = default;

    optional_copy_base(const optional_copy_base& other) {
	this->construct_from(other);
    }

    optional_copy_base(optional_copy_base&&) = default;
    optional_copy_base& operator=(const optional_copy_base&) = default;
    optional_copy_base& operator=(optional_copy_base&&) = default;
};

template <typename T, bool = std::is_trivially_move_constructible_v<T>>
struct optional_move_base : optional_copy_base<T> {
    using optional_copy_base<T>::optional_copy_base;
};

template <typename T>
struct optional_move_base<T, false> : optional_copy_base<T> {
    using value_type = T;
    using optional_copy_base<T>::optional_copy_base;

    optional_move_base() = default;
    optional_move_base(const optional_move_base&) = default;

    optional_move_base(optional_move_base&& other)
	noexcept(std::is_nothrow_move_constructible_v<value_type>) {
	this->construct_from(std::move(other));
    }

    optional_move_base& operator=(const optional_move_base&) = default;
    optional_move_base& operator=(optional_move_base&&) = default;
};

template <typename T, bool =
    std::is_trivially_destructible_v<T> &&
    std::is_trivially_copy_constructible_v<T> &&
    std::is_trivially_copy_assignable_v<T>
>
struct optional_copy_assign_base : optional_move_base<T> {
    using optional_move_base<T>::optional_move_base;
};

template <typename T>
struct optional_copy_assign_base<T, false> : optional_move_base<T> {
    using optional_move_base<T>::optional_move_base;

    optional_copy_assign_base() = default;
    optional_copy_assign_base(const optional_copy_assign_base&) = default;
    optional_copy_assign_base(optional_copy_assign_base&&) = default;

    optional_copy_assign_base& operator=(const optional_copy_assign_base& other) {
	this->assign_from(other);
	return *this;
    }

    optional_copy_assign_base& operator=(optional_copy_assign_base&&) = default;
};

template <typename T, bool =
    std::is_trivially_destructible_v<T> &&
    std::is_trivially_move_constructible_v<T> &&
    std::is_trivially_move_assignable_v<T>
>
struct optional_move_assign_base : optional_copy_assign_base<T> {
    using optional_copy_assign_base<T>::optional_copy_assign_base;
};

template <typename T>
struct optional_move_assign_base<T, false> : optional_copy_assign_base<T> {
    using value_type = T;
    using optional_copy_assign_base<T>::optional_copy_assign_base;

    optional_move_assign_base() = default;
    optional_move_assign_base(const optional_move_assign_base&) = default;
    optional_move_assign_base(optional_move_assign_base&&) = default;
    optional_move_assign_base& operator=(const optional_move_assign_base&) = default;

    optional_move_assign_base& operator=(optional_move_assign_base&& other)
	noexcept(std::is_nothrow_move_constructible_v<value_type> &&
		 std::is_nothrow_move_assignable_v<value_type>) {
	this->assign_from(std::move(other));
	return *this;
    }
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
class optional : private detail::optional_move_assign_base<T>,
		 private detail::optional_delete_ctor_base<T>,
		 private detail::optional_delete_assign_base<T> {
    using base = detail::optional_move_assign_base<T>;

 public:
    using value_type = T;

    constexpr optional() noexcept = default;

    constexpr optional(nullopt_t) noexcept {}

    constexpr optional(const optional& rhs) = default;
    constexpr optional(optional&& rhs) = default;

    template <typename ...Args>
    constexpr explicit optional(std::in_place_t, Args&&... args)
	: base(std::in_place, std::forward<Args>(args)...) {}

    template <typename U,
        std::enable_if_t<std::is_constructible_v<value_type, const U&>>* = nullptr,
        std::enable_if_t<std::is_convertible_v<const U&, value_type>>* = nullptr,
        detail::optional_enable_converting_constructors_t<value_type, U>* = nullptr
    >
    optional(const optional<U>& other) {
	this->construct_from(other);
    }

    template <typename U,
        std::enable_if_t<std::is_constructible_v<value_type, const U&>>* = nullptr,
        std::enable_if_t<!std::is_convertible_v<const U&, value_type>>* = nullptr,
        detail::optional_enable_converting_constructors_t<value_type, U>* = nullptr
    >
    explicit optional(const optional<U>& other) {
	this->construct_from(other);
    }

    template <typename U,
        std::enable_if_t<std::is_constructible_v<value_type, U&&>>* = nullptr,
        std::enable_if_t<std::is_convertible_v<U&&, value_type>>* = nullptr,
        detail::optional_enable_converting_constructors_t<value_type, U>* = nullptr
    >
    optional(optional<U>&& other) {
	this->construct_from(std::move(other));
    }

    template <typename U,
        std::enable_if_t<std::is_constructible_v<value_type, U&&>>* = nullptr,
        std::enable_if_t<!std::is_convertible_v<U&&, value_type>>* = nullptr,
        detail::optional_enable_converting_constructors_t<value_type, U>* = nullptr
    >
    explicit optional(optional<U>&& other) {
	this->construct_from(std::move(other));
    }

    template <typename U = value_type,
        std::enable_if_t<std::is_constructible_v<value_type, U&&>>* = nullptr,
        std::enable_if_t<std::is_convertible_v<U&&, value_type>>* = nullptr,
        detail::enable_forward_value_t<value_type, U>* = nullptr
    >
    constexpr optional(U&& value) : base(std::in_place, std::forward<U>(value)) {}

    template <typename U = value_type,
        std::enable_if_t<std::is_constructible_v<value_type, U&&>>* = nullptr,
        std::enable_if_t<!std::is_convertible_v<U&&, value_type>>* = nullptr,
        detail::enable_forward_value_t<value_type, U>* = nullptr
    >
    constexpr explicit optional(U&& value)
	: base(std::in_place, std::forward<U>(value)) {}

    optional& operator=(nullopt_t) noexcept {
	reset();
	return *this;
    }

    optional& operator=(const optional& rhs) = default;
    optional& operator=(optional&& rhs) = default;

    template <typename U,
        std::enable_if_t<std::is_constructible_v<value_type, const U&>>* = nullptr,
        std::enable_if_t<std::is_assignable_v<value_type&, const U&>>* = nullptr,
        detail::optional_enable_converting_assignment_t<value_type, U>* = nullptr
    >
    optional& operator=(const optional<U>& other) {
	this->assign_from(other);
	return *this;
    }

    template <typename U,
        std::enable_if_t<std::is_constructible_v<value_type, U>>* = nullptr,
        std::enable_if_t<std::is_assignable_v<value_type&, U>>* = nullptr,
        detail::optional_enable_converting_assignment_t<value_type, U>* = nullptr
    >
    optional& operator=(optional<U>&& other) {
	this->assign_from(std::move(other));
	return *this;
    }

    template <typename U = value_type,
        detail::optional_enable_forward_assignment_t<value_type, U>* = nullptr
    >
    optional& operator=(U&& value) {
	reset();
	this->construct(std::forward<U>(value));
	return *this;
    }

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

    template <typename U>
    constexpr value_type value_or(U&& default_value) const & {
	if (has_value())
	    return this->m_value;

	return static_cast<value_type>(std::forward<U>(default_value));
    }

    template <typename U>
    constexpr value_type value_or(U&& default_value) && {
	if (has_value())
	    return std::move(this->m_value);

	return static_cast<value_type>(std::forward<U>(default_value));
    }

    void reset() noexcept { this->destroy(); }

    template <typename ...Args>
    value_type& emplace(Args&&... args) {
	reset();
	this->construct(std::forward<Args>(args)...);

	return this->m_value;
    }

    template <typename F>
    constexpr auto then(F&& f) & {
	static_assert(std::is_invocable_v<F, value_type&>,
		      "F must be invocable with value_type&");

	using result_t = std::invoke_result_t<F, value_type&>;

	if constexpr (is_optional_v<result_t>) {
	    if (has_value())
		return std::invoke(std::forward<F>(f), **this);

	    return result_t(nullopt);
	}
	else {
	    if (has_value())
		return optional<result_t>(std::invoke(std::forward<F>(f), **this));

	    return optional<result_t>(nullopt);
	}
    }

    template <typename F>
    constexpr auto then(F&& f) && {
	static_assert(std::is_invocable_v<F, value_type&&>,
		      "F must be invocable with value_type&&");

	using result_t = std::invoke_result_t<F, value_type&&>;

	if constexpr (is_optional_v<result_t>) {
	    if (has_value())
		return std::invoke(std::forward<F>(f), **this);

	    return result_t(nullopt);
	}
	else {
	    if (has_value())
		return optional<result_t>(std::invoke(std::forward<F>(f), **this));

	    return optional<result_t>(nullopt);
	}
    }

    template <typename F>
    constexpr auto then(F&& f) const & {
	static_assert(std::is_invocable_v<F, const value_type&>,
		      "F must be invocable with const value_type&");

	using result_t = std::invoke_result_t<F, const value_type&>;

	if constexpr (is_optional_v<result_t>) {
	    if (has_value())
		return std::invoke(std::forward<F>(f), **this);

	    return result_t(nullopt);
	}
	else {
	    if (has_value())
		return optional<result_t>(std::invoke(std::forward<F>(f), **this));

	    return optional<result_t>(nullopt);
	}
    }

    template <typename F>
    constexpr auto then(F&& f) const && {
	static_assert(std::is_invocable_v<F, const value_type&>,
		      "F must be invocable with const value_type&");

	using result_t = std::invoke_result_t<F, const value_type&>;

	if constexpr (is_optional_v<result_t>) {
	    if (has_value())
		return std::invoke(std::forward<F>(f), **this);

	    return result_t(nullopt);
	}
	else {
	    if (has_value())
		return optional<result_t>(std::invoke(std::forward<F>(f), **this));

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
	    return optional<result_t>(std::invoke(std::forward<F>(f)));
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
	    return optional<result_t>(std::invoke(std::forward<F>(f)));
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
	    return optional<result_t>(std::invoke(std::forward<F>(f)));
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
	    return optional<result_t>(std::invoke(std::forward<F>(f)));
	}
    }
};

} // namespace umlaut
