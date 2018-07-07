/// @file
/// Defines umlaut::small_vector.
///
/// @copyright Marcus Larsson 2018
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#pragma once

#include "compressed_pair.hpp"
#include "tags.hpp"

#include <memory>
#include <iterator>
#include <utility>
#include <tuple>
#include <cstddef>

namespace umlaut {

/// Generic container.
template <typename T, typename Alloc = std::allocator<T>>
class small_vector_base {
    using alloc_traits = typename std::allocator_traits<Alloc>::template rebind_traits<T>;

public:
    /// @name Aliases
    /// @{
    using value_type = T;
    using allocator_type = typename alloc_traits::allocator_type;
    using size_type = typename alloc_traits::size_type;
    using difference_type = typename alloc_traits::difference_type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = typename alloc_traits::pointer;
    using const_pointer = typename alloc_traits::const_pointer;
    using iterator = value_type*;
    using const_iterator = const value_type*;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    /// @}

    /// @brief Constructs the `vector` from a list of values.
    template <typename ...Ts>
    small_vector_base(list_construct_t, Ts&&... values) {
	reserve(sizeof...(values));
	(emplace_back(std::forward<Ts>(values)), ...);
    }

    template <typename ...Tuples>
    small_vector_base(std::piecewise_construct_t, Tuples&&... tuples) {
	reserve(sizeof...(tuples));

	auto forwarding_lambda = [this](auto&&... args) {
	    emplace_back(std::forward<decltype(args)>(args)...);
	};

	(std::apply(forwarding_lambda, tuples), ...);
    }

    /// @name Element access
    /// @{

    /// @brief Returns element at index `i`.
    constexpr value_type& operator[](size_type i) { return data()[i]; }

    /// @brief Const overload of `vector::operator[]`.
    constexpr const value_type& operator[](size_type i) const { return data()[i]; }

    /// @brief Returns a pointer to the underlying data of the `vector`.
    constexpr value_type* data() noexcept {
	return static_cast<value_type*>(m_data());
    }

    /// @brief Const overload of vector::data().
    constexpr const value_type* data() const noexcept {
	return static_cast<value_type*>(m_data());
    }
    /// @}

    /// @name Iterators
    /// @{
    constexpr iterator begin() noexcept { return &data()[0]; }
    constexpr const_iterator cbegin() noexcept { return &data()[0]; }
    constexpr const_iterator begin() const noexcept { return &data()[0]; }
    constexpr iterator end() noexcept { return &data()[m_size]; }
    constexpr const_iterator cend() noexcept { return &data()[m_size]; }
    constexpr const_iterator end() const noexcept { return &data()[m_size]; }
    constexpr reverse_iterator rbegin() noexcept { return &data()[m_size]; }
    constexpr const_reverse_iterator crbegin() noexcept { return &data()[m_size]; }
    constexpr const_reverse_iterator rbegin() const noexcept { return &data()[m_size]; }
    constexpr reverse_iterator rend() noexcept { return &data()[0]; }
    constexpr const_reverse_iterator crend() noexcept { return &data()[0]; }
    constexpr const_reverse_iterator rend() const noexcept { return &data()[0]; }
    /// @}

    /// @name Modifiers
    /// @{

    /// @brief Adds an element to the end of the `vector`.
    void push_back(const value_type& value) { emplace_back(value); }

    /// @brief Overload taking an rvalue reference.
    void push_back(value_type&& value) { emplace_back(std::move(value)); }

    /// @brief Constructs an element in place at the end of the vector by forwarding
    /// constructor arguments to the `value_type`, returns a reference to the constructed
    /// element.
    template <typename ...Args>
    value_type& emplace_back(Args&&... args) {
	// TODO ...
    }
    /// @}

    /// @name Capacity
    /// @{
    void reserve(size_type n) {
	// TODO ...
    }

    /// @brief Returns the numbers of elements in the `vector`.
    constexpr size_type size() const noexcept { return m_size; }

    /// @brief Returns the potential number of elements in the `vector` before the
    /// `vector has to reallocate.
    constexpr size_type capacity() const noexcept { return m_capacity; }

    /// @brief Returns whether the `vector` is empty of not.
    constexpr bool empty() const noexcept { return m_size == 0; }

    /// @brief Returns the maximum size the can have vector.
    constexpr size_type max_size() const noexcept { return alloc_traits::max_size(m_alloc()); }
    /// @}

 private:
    compressed_pair<pointer, allocator_type> m_data_and_alloc;
    size_type m_size;
    size_type m_capacity;

    pointer& m_data() noexcept { return m_data_and_alloc.first(); }
    const_pointer& m_data() const noexcept { return m_data_and_alloc.first(); }

    allocator_type& m_alloc() noexcept { return m_data_and_alloc.second(); }
    const allocator_type& m_alloc() const noexcept { return m_data_and_alloc.second(); }
};

} // namespace umlaut
