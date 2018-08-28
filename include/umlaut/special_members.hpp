// @copyright Marcus Larsson 2018
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#pragma once

namespace umlaut::detail {

template <bool EnableCopy, bool EnableMove>
struct delete_ctor_base {
    delete_ctor_base() = default;
    delete_ctor_base(const delete_ctor_base&) = default;
    delete_ctor_base(delete_ctor_base&&) noexcept = default;
    delete_ctor_base& operator=(const delete_ctor_base&) = default;
    delete_ctor_base& operator=(delete_ctor_base&&) noexcept = default;
};

template <>
struct delete_ctor_base<false, false> {
    delete_ctor_base() = default;
    delete_ctor_base(const delete_ctor_base&) = delete;
    delete_ctor_base(delete_ctor_base&&) noexcept = delete;
    delete_ctor_base& operator=(const delete_ctor_base&) = default;
    delete_ctor_base& operator=(delete_ctor_base&&) noexcept = default;
};

template <>
struct delete_ctor_base<true, false> {
    delete_ctor_base() = default;
    delete_ctor_base(const delete_ctor_base&) = default;
    delete_ctor_base(delete_ctor_base&&) noexcept = delete;
    delete_ctor_base& operator=(const delete_ctor_base&) = default;
    delete_ctor_base& operator=(delete_ctor_base&&) noexcept = default;
};

template <>
struct delete_ctor_base<false, true> {
    delete_ctor_base() = default;
    delete_ctor_base(const delete_ctor_base&) = delete;
    delete_ctor_base(delete_ctor_base&&) noexcept = default;
    delete_ctor_base& operator=(const delete_ctor_base&) = default;
    delete_ctor_base& operator=(delete_ctor_base&&) noexcept = default;
};


template <bool EnableCopy, bool EnableMove>
struct delete_assign_base {
    delete_assign_base() = default;
    delete_assign_base(const delete_assign_base&) = default;
    delete_assign_base(delete_assign_base&&) noexcept = default;
    delete_assign_base& operator=(const delete_assign_base&) = default;
    delete_assign_base& operator=(delete_assign_base&&) noexcept = default;
};

template <>
struct delete_assign_base<false, false> {
    delete_assign_base() = default;
    delete_assign_base(const delete_assign_base&) = default;
    delete_assign_base(delete_assign_base&&) noexcept = default;
    delete_assign_base& operator=(const delete_assign_base&) = delete;
    delete_assign_base& operator=(delete_assign_base&&) noexcept = delete;
};

template <>
struct delete_assign_base<true, false> {
    delete_assign_base() = default;
    delete_assign_base(const delete_assign_base&) = default;
    delete_assign_base(delete_assign_base&&) noexcept = default;
    delete_assign_base& operator=(const delete_assign_base&) = default;
    delete_assign_base& operator=(delete_assign_base&&) noexcept = delete;
};

template <>
struct delete_assign_base<false, true> {
    delete_assign_base() = default;
    delete_assign_base(const delete_assign_base&) = default;
    delete_assign_base(delete_assign_base&&) noexcept = default;
    delete_assign_base& operator=(const delete_assign_base&) = delete;
    delete_assign_base& operator=(delete_assign_base&&) noexcept = default;
};

} // namespace umlaut::detail
