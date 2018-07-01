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
#include <cstddef>

namespace umlaut {

/// Generic container.
template <typename T, typename Alloc = std::allocator<T>>
class small_vector_base {
    using alloc_traits = typename std::allocator_traits<Alloc>::template rebind_traits<T>;

public:
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

    /// Constructs the `vector` from a list of values.
    template <typename ...Ts>
    small_vector_base(list_construct_t, Ts&&... values) {
	// TODO ...
    }

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
