#include <gtest/gtest.h>
#include <umlaut.hpp>

struct empty_cat {
    const char* purr() const { return "purr"; }
};

TEST(compressedPair, size) {
    umlaut::compressed_pair<int, int> largest{1, 1};
    umlaut::compressed_pair<int, empty_cat> middle{1, {}};
    umlaut::compressed_pair<empty_cat, empty_cat> smallest{{}, {}};

    EXPECT_TRUE(sizeof(largest) > sizeof(middle));
    EXPECT_TRUE(sizeof(middle) > sizeof(smallest));
}

TEST(compressedPair, access) {
    umlaut::compressed_pair<int, empty_cat> pair{42, {}};

    EXPECT_EQ(pair.first(), 42);
    EXPECT_EQ(pair.second().purr(), "purr");
}
