// Copyright Marcus Larsson 2018
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include <catch2/catch.hpp>
#include <umlaut/small_vector.hpp>

TEST_CASE("construction of small_vector_base", "[small_vector_base]") {
    struct type {
	type(int i, int j) : m_i(i), m_j(j) {}
	int m_i, m_j;
    };

    SECTION("list_initialization") {
	ul::small_vector_base<int> v(ul::list_construct, 1, 2, 3);

	CHECK(v[0] == 1);
	CHECK(v[1] == 2);
	CHECK(v[2] == 3);
    }

    SECTION("list_initialization_alloc") {
	std::allocator<int> alloc;
	ul::small_vector_base<int> v(ul::list_construct, alloc, 1 , 2, 3);

	CHECK(v[0] == 1);
	CHECK(v[1] == 2);
	CHECK(v[2] == 3);
    }

    SECTION("piecewise_constuct") {
	ul::small_vector_base<type> v(std::piecewise_construct,
				      std::forward_as_tuple(1, 2),
				      std::forward_as_tuple(3, 4));

	CHECK(v[0].m_i == 1);
	CHECK(v[0].m_j == 2);
	CHECK(v[1].m_i == 3);
	CHECK(v[1].m_j == 4);
    }

    SECTION("piecewise_constuct_alloc") {
	std::allocator<type> alloc;
	ul::small_vector_base<type> v(std::piecewise_construct, alloc,
				      std::forward_as_tuple(1, 2),
				      std::forward_as_tuple(3, 4));

	CHECK(v[0].m_i == 1);
	CHECK(v[0].m_j == 2);
	CHECK(v[1].m_i == 3);
	CHECK(v[1].m_j == 4);
    }
}
