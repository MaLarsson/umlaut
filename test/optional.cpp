// Copyright Marcus Larsson 2018
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <umlaut/optional.hpp>
#include <type_traits>
#include <string>

namespace {

TEST(optional, isTriviallyDestructable) {
    struct non_trivial_type {
	~non_trivial_type() {}
    };

    ul::optional<int> trivial{};
    ul::optional<non_trivial_type> non_trivial{};

    EXPECT_TRUE(std::is_trivially_destructible_v<decltype(trivial)>);
    EXPECT_FALSE(std::is_trivially_destructible_v<decltype(non_trivial)>);
}

TEST(optional, hasValue) {
    ul::optional<int> initialized{std::in_place, 1};
    ul::optional<int> non_initialized{};

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

    ul::optional<copyable_type> copyable;
    ul::optional<non_copyable_type> non_copyable;

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

    ul::optional<movable_type> movable;
    ul::optional<non_movable_type> non_movable;

    EXPECT_TRUE(std::is_move_constructible_v<decltype(movable)>);
    EXPECT_TRUE(std::is_nothrow_move_constructible_v<decltype(movable)>);
    EXPECT_FALSE(std::is_move_constructible_v<decltype(non_movable)>);
}

int add_three(int value) { return value + 3; }

TEST(optional, thenSuccess) {
    auto get_optional = []() ->ul::optional<int> {
	return ul::optional<int>(std::in_place, 1);
    };

    auto add_ten = [](auto&& value) ->ul::optional<int> {
	return ul::optional<int>(std::in_place, value + 10);
    };

    auto result = get_optional()
      .then([](auto&& value) { return ul::optional<int>(std::in_place, value + 5); })
      .then([](auto&& value) { return value + 1; })
      .then(add_ten)
      .then(&add_three);

    EXPECT_EQ(result.value(), 20);
}

TEST(optional, thenFail) {
    int initial_value = 0;
    bool post_fail_invoked = false;

    auto result = ul::optional<int>(std::in_place, 2)
      .then([&](auto&& value) { initial_value = value; return value; })
      .then([](auto&&) { return ul::optional<int>(ul::nullopt); })
      .then([&](auto&& value) { post_fail_invoked = true; return value; });

    EXPECT_FALSE(post_fail_invoked);
    EXPECT_EQ(initial_value, 2);
    EXPECT_FALSE(result.has_value());
}

TEST(optional, catchError) {
    bool first_catch_invoked = false;
    bool second_catch_invoked = false;

    auto first_result = ul::optional<int>(std::in_place, 0)
      .then([](auto&&) { return ul::optional<int>(ul::nullopt); })
      .catch_error([&]() { first_catch_invoked = true; });

    auto second_result = ul::optional<int>(std::in_place, 0)
      .then([](auto&& value) { return value + 1; })
      .catch_error([&]() { second_catch_invoked = true; });

    EXPECT_TRUE(first_catch_invoked);
    EXPECT_FALSE(first_result.has_value());
    EXPECT_FALSE(second_catch_invoked);
    EXPECT_TRUE(second_result.has_value());
}

TEST(optional, reset) {
    ul::optional<int> opt{std::in_place, 5};

    EXPECT_TRUE(opt.has_value());

    opt.reset();

    EXPECT_FALSE(opt.has_value());
}

TEST(optional, emplace) {
    struct type {
	type(int i, double d) : i(i), d(d) {}
	int i; double d;
    };

    ul::optional<int> opt1{std::in_place, 5};
    ul::optional<type> opt2{};

    opt1.emplace(10);
    opt2.emplace(20, 1.1);

    EXPECT_EQ(opt1.value(), 10);
    EXPECT_EQ(opt2.value().i, 20);
    EXPECT_EQ(opt2.value().d, 1.1);
}

TEST(optional, convertingCopyConstructor) {
    ul::optional<int> int_optional{std::in_place, 1};
    ul::optional<double> double_optional{int_optional};

    bool double_to_int = std::is_constructible_v<ul::optional<int>, ul::optional<double>>;
    bool int_to_string = std::is_constructible_v<ul::optional<std::string>, ul::optional<int>>;

    EXPECT_EQ(int_optional.value(), double_optional.value());
    EXPECT_TRUE(double_to_int);
    EXPECT_FALSE(int_to_string);
}

TEST(optional, convertingConstructor) {
    bool double_to_int = std::is_constructible_v<ul::optional<int>, double>;
    bool int_to_string = std::is_constructible_v<ul::optional<std::string>, int>;

    EXPECT_TRUE(double_to_int);
    EXPECT_FALSE(int_to_string);
}

TEST(optional, convertingAssignment) {
    ul::optional<double> double_optional(3.33);
    double_optional = 1;

    EXPECT_TRUE(double_optional.has_value());
    EXPECT_EQ(double_optional.value(), 1);
}

TEST(optional, swapBothHasValue) {
    ul::optional<int> opt1{1};
    ul::optional<int> opt2{2};

    opt1.swap(opt2);

    EXPECT_EQ(opt1.value(), 2);
    EXPECT_EQ(opt2.value(), 1);
}

TEST(optional, swapOneHasValue) {
    ul::optional<int> opt1{1};
    ul::optional<int> opt2{ul::nullopt};

    opt1.swap(opt2);

    EXPECT_FALSE(opt1.has_value());
    EXPECT_TRUE(opt2.has_value());
    EXPECT_EQ(opt2.value(), 1);

    opt1.swap(opt2);

    EXPECT_FALSE(opt2.has_value());
    EXPECT_TRUE(opt1.has_value());
    EXPECT_EQ(opt1.value(), 1);
}

TEST(optional, adlSwap) {
    ul::optional<int> opt1{1};
    ul::optional<int> opt2{2};

    using std::swap;
    swap(opt1, opt2);

    EXPECT_EQ(opt1.value(), 2);
    EXPECT_EQ(opt2.value(), 1);
}

TEST(optional, deductionGuides) {
    ul::optional int_opt{1};

    bool is_int_opt = std::is_same_v<ul::optional<int>, decltype(int_opt)>;

    int array[2];
    ul::optional array_opt{array};

    bool is_array_opt = std::is_same_v<ul::optional<std::decay_t<decltype(array)>>,
				       decltype(array_opt)>;

    EXPECT_TRUE(is_int_opt);
    EXPECT_TRUE(is_array_opt);
}

} // namespace
