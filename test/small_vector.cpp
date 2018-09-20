// Copyright Marcus Larsson 2018
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <umlaut/small_vector.hpp>

namespace {

TEST(smallVectorBase, listInitialization) {
    ul::small_vector_base<int> v(ul::list_construct, 1, 2, 3);

    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 3);
}

TEST(smallVectorBase, listInitializationAlloc) {
    std::allocator<int> alloc;
    ul::small_vector_base<int> v(ul::list_construct, alloc, 1 , 2, 3);

    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 3);
}

TEST(smallVectorBase, piecewiseConstuct) {
    struct type {
	type(int i, int j) : m_i(i), m_j(j) {}
	int m_i, m_j;
    };

    ul::small_vector_base<type> v(std::piecewise_construct,
				  std::forward_as_tuple(1, 2),
				  std::forward_as_tuple(3, 4));
}

TEST(smallVectorBase, piecewiseConstuctAlloc) {
    struct type {
	type(int i, int j) : m_i(i), m_j(j) {}
	int m_i, m_j;
    };

    std::allocator<type> alloc;
    ul::small_vector_base<type> v(std::piecewise_construct, alloc,
				  std::forward_as_tuple(1, 2),
				  std::forward_as_tuple(3, 4));
}

} // namespace
