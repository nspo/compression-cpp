#include <gtest/gtest.h>
#include <stack>
#include "ShortBitSet.h"

TEST(shortbitset, basic) { // NOLINT
    ShortBitSet bs;
    EXPECT_EQ(bs.size(), 0);
    EXPECT_ANY_THROW((void) bs.at(0));
    EXPECT_ANY_THROW(bs.pop_back());

    bs.push_back(true);
    EXPECT_EQ(bs.size(), 1);
    EXPECT_EQ(bs.at(0), true);
    EXPECT_EQ(bs.pop_back(), true);

    // add 1010101010
    for (int i=0; i<5; ++i) {
        bs.push_back(true);
        bs.push_back(false);
    }

    EXPECT_EQ(bs.size(), 10);
    for(size_t i=0; i<5; ++i) {
        EXPECT_EQ(bs.at(2*i), true);
        EXPECT_EQ(bs.at(2*i+1), false);
    }
    for(size_t i=0; i<bs.size(); ++i) {
        if (i%2 == 0) EXPECT_EQ(bs.at(i), true);
        else EXPECT_EQ(bs.at(i), false);
    }

    bs.clear();
    EXPECT_EQ(bs.size(), 0);
    for (int i=0; i<16; ++i) {
        bs.push_back(false);
        bs.push_back(true);
    }
    EXPECT_EQ(bs.size(), 32);
    EXPECT_ANY_THROW(bs.push_back(true));
    EXPECT_EQ(bs.size(), 32);

    for (int i=0; i<16; ++i) {
        EXPECT_EQ(bs.pop_back(), true);
        EXPECT_EQ(bs.pop_back(), false);
    }
    EXPECT_EQ(bs.empty(), true);
}
