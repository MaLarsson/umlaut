// Copyright Marcus Larsson 2018
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <umlaut/optional.hpp>
#include <type_traits>

namespace {

TEST(optional, isTriviallyDestructable) {
    struct non_trivial_type {
	~non_trivial_type() {}
    };

    umlaut::optional<int> trivial{};
    umlaut::optional<non_trivial_type> non_trivial{};

    EXPECT_TRUE(std::is_trivially_destructible_v<decltype(trivial)>);
    EXPECT_FALSE(std::is_trivially_destructible_v<decltype(non_trivial)>);
}

TEST(optional, hasValue) {
    umlaut::optional<int> initialized{std::in_place, 1};
    umlaut::optional<int> non_initialized{};

    EXPECT_TRUE(initialized.has_value());
    EXPECT_FALSE(non_initialized.has_value());
}

} // namespace
