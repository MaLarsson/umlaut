// Copyright Marcus Larsson 2018
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <umlaut/compressed_pair.hpp>
#include <string>

namespace {

struct empty_cat {
    std::string purr() const { return "purr"; }
};

TEST(compressedPair, size) {
    ul::compressed_pair<int, int> largest{1, 1};
    ul::compressed_pair<int, empty_cat> middle{1, {}};
    ul::compressed_pair<empty_cat, empty_cat> smallest{{}, {}};

    EXPECT_TRUE(sizeof(largest) > sizeof(middle));
    EXPECT_TRUE(sizeof(middle) > sizeof(smallest));
}

TEST(compressedPair, access) {
    ul::compressed_pair<int, empty_cat> pair{42, {}};

    EXPECT_EQ(pair.first(), 42);
    EXPECT_EQ(pair.second().purr(), "purr");
}

} // namespace
