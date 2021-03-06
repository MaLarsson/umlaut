// Copyright Marcus Larsson 2018
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include <catch2/catch.hpp>
#include <umlaut/compressed_pair.hpp>
#include <type_traits>
#include <utility>
#include <string>

struct empty_cat { std::string purr() const { return "purr"; } };

TEST_CASE("access functions for compressed_pair", "[compressed_pair]") {
    ul::compressed_pair<int, empty_cat> pair{42, {}};

    CHECK(pair.first() == 42);
    CHECK(pair.second().purr() == "purr");
}

TEST_CASE("access overload resolution from compressed_pair", "[compressed_pair]") {
    using T = ul::compressed_pair<int, int>;

    CHECK(std::is_same_v<decltype(std::declval<T&>().first()), int&>);
    CHECK(std::is_same_v<decltype(std::declval<T&&>().first()), int&&>);

    CHECK(std::is_same_v<decltype(std::declval<const T&>().first()), const int&>);
    CHECK(std::is_same_v<decltype(std::declval<const T&&>().first()), const int&&>);

    CHECK(std::is_same_v<decltype(std::declval<T&>().second()), int&>);
    CHECK(std::is_same_v<decltype(std::declval<T&&>().second()), int&&>);

    CHECK(std::is_same_v<decltype(std::declval<const T&>().second()), const int&>);
    CHECK(std::is_same_v<decltype(std::declval<const T&&>().second()), const int&&>);
}

TEST_CASE("empty base optimization of compressed_pair", "[compressed_pair][ebo]") {
    ul::compressed_pair<int, int> largest{1, 1};
    ul::compressed_pair<int, empty_cat> middle{1, {}};
    ul::compressed_pair<empty_cat, empty_cat> smallest{{}, {}};

    CHECK(sizeof(largest) > sizeof(middle));
    CHECK(sizeof(middle) > sizeof(smallest));
}
