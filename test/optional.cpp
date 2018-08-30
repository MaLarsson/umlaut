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

TEST(optional, propagateCopyConstruction) {
    struct copyable_type {
	copyable_type() = default;
	copyable_type(const copyable_type&) = default;
	copyable_type(copyable_type&&) noexcept = default;
	copyable_type& operator=(const copyable_type&) = default;
	copyable_type& operator=(copyable_type&&) noexcept = default;
    };

    struct non_copyable_type {
	non_copyable_type() = default;
	non_copyable_type(const non_copyable_type&) = delete;
	non_copyable_type(non_copyable_type&&) noexcept = default;
	non_copyable_type& operator=(const non_copyable_type&) = default;
	non_copyable_type& operator=(non_copyable_type&&) noexcept = default;
    };

    umlaut::optional<copyable_type> copyable;
    umlaut::optional<non_copyable_type> non_copyable;

    EXPECT_TRUE(std::is_copy_constructible_v<decltype(copyable)>);
    EXPECT_FALSE(std::is_copy_constructible_v<decltype(non_copyable)>);
}

TEST(optional, propagateMoveConstruction) {
    struct movable_type {
	movable_type() = default;
	movable_type(const movable_type&) = default;
	movable_type(movable_type&&) noexcept = default;
	movable_type& operator=(const movable_type&) = default;
	movable_type& operator=(movable_type&&) noexcept = default;
    };

    struct non_movable_type {
	non_movable_type() = default;
	non_movable_type(const non_movable_type&) = delete;
	non_movable_type(non_movable_type&&) noexcept = delete;
	non_movable_type& operator=(const non_movable_type&) = default;
	non_movable_type& operator=(non_movable_type&&) noexcept = default;
    };

    umlaut::optional<movable_type> movable;
    umlaut::optional<non_movable_type> non_movable;

    EXPECT_TRUE(std::is_move_constructible_v<decltype(movable)>);
    EXPECT_TRUE(std::is_nothrow_move_constructible_v<decltype(movable)>);
    EXPECT_FALSE(std::is_move_constructible_v<decltype(non_movable)>);
}

int add_three(int value) { return value + 3; }

TEST(optional, thenSuccess) {
    auto get_optional = []() ->umlaut::optional<int> {
	return { std::in_place, 1 };
    };

    auto add_ten = [](auto&& value) ->umlaut::optional<int> {
	return { std::in_place, value + 10 };
    };

    auto result = get_optional()
      .then([](auto&& value) ->umlaut::optional<int> { return { std::in_place, value + 5 }; })
      .then([](auto&& value) { return value + 1; })
      .then(add_ten)
      .then(&add_three);

    EXPECT_EQ(result.value(), 20);
}

TEST(optional, thenFail) {
    int initial_value = 0;
    bool post_fail_invoked = false;

    auto result = umlaut::optional<int>(std::in_place, 2)
      .then([&](auto&& value) { initial_value = value; return value; })
      .then([](auto&&) { return umlaut::optional<int>(umlaut::nullopt); })
      .then([&](auto&& value) { post_fail_invoked = true; return value; });

    EXPECT_FALSE(post_fail_invoked);
    EXPECT_EQ(initial_value, 2);
    EXPECT_FALSE(result.has_value());
}

} // namespace
