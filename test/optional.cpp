// Copyright Marcus Larsson 2018
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include <catch2/catch.hpp>
#include <umlaut/optional.hpp>
#include <type_traits>
#include <string>

TEST_CASE("progagate traits from underlying type for optional", "[optional]") {
    struct non_trivial_type {
	~non_trivial_type() {}
    };

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

    ul::optional<int> trivial;
    ul::optional<non_trivial_type> non_trivial;
    ul::optional<copyable_type> copyable;
    ul::optional<non_copyable_type> non_copyable;
    ul::optional<movable_type> movable;
    ul::optional<non_movable_type> non_movable;

    SECTION("is trivially destructable") {
	CHECK(std::is_trivially_destructible_v<decltype(trivial)>);
	CHECK_FALSE(std::is_trivially_destructible_v<decltype(non_trivial)>);
    }

    SECTION("copy construction") {
	CHECK(std::is_copy_constructible_v<decltype(copyable)>);
	CHECK_FALSE(std::is_copy_constructible_v<decltype(non_copyable)>);
    }

    SECTION("move construction") {
	CHECK(std::is_move_constructible_v<decltype(movable)>);
	CHECK(std::is_nothrow_move_constructible_v<decltype(movable)>);
	CHECK_FALSE(std::is_move_constructible_v<decltype(non_movable)>);
    }
}

int add_three(int value) { return value + 3; }

TEST_CASE("use the monadic interface of optional", "[optional][monads]") {
    ul::optional opt{1};

    auto add_ten = [](auto&& value) ->ul::optional<int> { return value + 10; };

    SECTION("single then successfully") {
	auto result = opt.then([](auto&& value) { return value + 1; });

	CHECK(result.has_value());
	CHECK(result.value() == 2);
    }

    SECTION("single then failed") {
	auto result = opt.then([](auto&&) { return ul::nullopt; });

	CHECK_FALSE(result.has_value());
    }

    SECTION("chaining then successfully") {
	auto result = opt
	  .then([](auto&& value) { return ul::optional<int>(std::in_place, value + 5); })
	  .then([](auto&& value) { return value + 1; })
	  .then(add_ten)
	  .then(&add_three);

	CHECK(result.has_value());
	CHECK(result.value() == 20);
    }

    SECTION("chaining then failed") {
	int initial_value = 0;
	bool post_fail_invoked = false;

	auto result = opt
	  .then([&](auto&& value) { initial_value = value; return value; })
	  .then([](auto&&) { return ul::nullopt; })
	  .then([&](auto&& value) { post_fail_invoked = true; return value; });

	CHECK_FALSE(post_fail_invoked);
	CHECK(initial_value == 1);
	CHECK_FALSE(result.has_value());
    }

    SECTION("not invoking catch_error after then") {
	bool catch_invoked = false;

	auto result = opt
	  .then([](auto&& value) { return value; })
	  .catch_error([&]() { catch_invoked = true; });

	CHECK_FALSE(catch_invoked);
	CHECK(result.has_value());
    }

    SECTION("invoking catch_error after then") {
	bool catch_invoked = false;

	auto result = opt
	  .then([](auto&&) { return ul::nullopt; })
	  .catch_error([&]() { catch_invoked = true; });

	CHECK(catch_invoked);
	CHECK_FALSE(result.has_value());
    }

    SECTION("not invoking catch_error without then") {
	bool catch_invoked = false;

	auto result = opt.catch_error([&]() { catch_invoked = true; });

	CHECK_FALSE(catch_invoked);
	CHECK(result.has_value());
    }

    SECTION("invoking catch_error without then") {
	bool catch_invoked = false;

	auto result = ul::optional<int>{ul::nullopt}
	  .catch_error([&]() { catch_invoked = true; });

	CHECK(catch_invoked);
	CHECK_FALSE(result.has_value());
    }
}

TEST_CASE("modification of optional", "[optional]") {
    ul::optional opt{1};
    ul::optional<int> null_opt{ul::nullopt};

    CHECK(opt.has_value());
    CHECK(opt.value() == 1);
    CHECK_FALSE(null_opt.has_value());

    SECTION("reset value of the optional") {
	opt.reset();
	null_opt.reset();

	CHECK_FALSE(opt.has_value());
	CHECK_FALSE(null_opt.has_value());
    }

    SECTION("emplace a builtin value into the optional") {
	opt.emplace(10);
	null_opt.emplace(20);

	CHECK(opt.has_value());
	CHECK(opt.value() == 10);

	CHECK(null_opt.has_value());
	CHECK(opt.value() == 10);
    }

    SECTION("emplace custom type into the optional") {
	struct type {
	    type(int i, double d) : i(i), d(d) {}
	    int i; double d;
	};

	ul::optional<type> type_opt{std::in_place, 1, 2.0};
	ul::optional<type> null_type_opt{ul::nullopt};

	type_opt.emplace(10, 20);
	null_type_opt.emplace(30, 40.5);

	CHECK(type_opt.has_value());
	CHECK(type_opt.value().i == 10);
	CHECK(type_opt.value().d == 20);

	CHECK(null_type_opt.has_value());
	CHECK(null_type_opt.value().i == 30);
	CHECK(null_type_opt.value().d == 40.5);
    }

    SECTION("swap with two optionals, both containing a value") {
	ul::optional opt2{2};

	opt.swap(opt2);

	CHECK(opt.has_value());
	CHECK(opt.value() == 2);

	CHECK(opt2.has_value());
	CHECK(opt2.value() == 1);
    }

    SECTION("swap with two optionals, only one containing a value") {
	opt.swap(null_opt);

	CHECK_FALSE(opt.has_value());
	CHECK(null_opt.has_value());
	CHECK(null_opt.value() == 1);

	opt.swap(null_opt);

	CHECK_FALSE(null_opt.has_value());
	CHECK(opt.has_value());
	CHECK(opt.value() == 1);
    }

    SECTION("swap with two optionals, neither containing a value") {
	ul::optional<int> null_opt2{ul::nullopt};

	null_opt.swap(null_opt2);

	CHECK_FALSE(null_opt.has_value());
	CHECK_FALSE(null_opt2.has_value());
    }

    SECTION("swaping using adl") {
	using std::swap;
	swap(opt, null_opt);

	CHECK_FALSE(opt.has_value());
	CHECK(null_opt.has_value());
	CHECK(null_opt.value() == 1);
    }
}

TEST_CASE("construction of optional", "[optional]") {
    SECTION("converting constructor") {
	CHECK(std::is_constructible_v<ul::optional<int>, double>);
	CHECK_FALSE(std::is_constructible_v<ul::optional<std::string>, int>);
    }

    SECTION("converting copy constructor") {
	ul::optional<int> int_opt{std::in_place, 1};
	ul::optional<double> double_opt{int_opt};

	CHECK(int_opt.has_value());
	CHECK(double_opt.has_value());
	CHECK(int_opt.value() == double_opt.value());

	CHECK(std::is_constructible_v<ul::optional<int>, ul::optional<double>>);
	CHECK_FALSE(std::is_constructible_v<ul::optional<std::string>, ul::optional<int>>);
    }

    SECTION("deduction guide for optional") {
	int array[2];

	ul::optional int_opt{1};
	ul::optional array_opt{array};

	CHECK(std::is_same_v<ul::optional<int>, decltype(int_opt)>);
	CHECK(std::is_same_v<ul::optional<std::decay_t<decltype(array)>>, decltype(array_opt)>);
    }
}

TEST_CASE("assignment of optional", "[optional]") {
    ul::optional opt{3.33};

    SECTION("converting assignment") {
	opt = 3; // implicit conversion from int to double

	CHECK(opt.has_value());
	CHECK(opt.value() == 3);
    }

    SECTION("assignment of nullopt") {
	opt = ul::nullopt;

	CHECK_FALSE(opt.has_value());
    }
}
