#include <gtest/gtest.h>
#include <umlaut.hpp>

struct empty_cat {
    const char* purr() const { return "purr"; }
};

TEST(pair, size) {
    umlaut::pair<int, int> largest{1, 1};
    umlaut::pair<int, empty_cat> middle{1, {}};
    umlaut::pair<empty_cat, empty_cat> smallest{{}, {}};

    EXPECT_TRUE(sizeof(largest) > sizeof(middle));
    EXPECT_TRUE(sizeof(middle) > sizeof(smallest));
}

TEST(pair, access) {
    umlaut::pair<int, empty_cat> pair{42, {}};

    EXPECT_EQ(pair.first(), 42);
    EXPECT_EQ(pair.second().purr(), "purr");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
