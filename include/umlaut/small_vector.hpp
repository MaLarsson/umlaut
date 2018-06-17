#pragma once

#include "compressed_pair.hpp"

#include <memory>
#include <iterator>
#include <utility>
#include <cstddef>

namespace umlaut {

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
