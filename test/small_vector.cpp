// Copyright Marcus Larsson 2018
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <umlaut.hpp>

namespace {

TEST(smallVectorBase, listInitialization) {
    umlaut::small_vector_base<int> v(umlaut::list_construct, 1, 2, 3);
}

TEST(smallVectorBase, piecewiseConstuct) {
    struct type {
	type(int i, int j) : m_i(i), m_j(j) {}
	int m_i, m_j;
    };

    umlaut::small_vector_base<type> v(std::piecewise_construct,
				      std::forward_as_tuple(1, 2),
				      std::forward_as_tuple(3, 4));
}

} // namespace
