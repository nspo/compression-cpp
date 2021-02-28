#include <gtest/gtest.h>
#include <stack>

#include "BitStreamIn.h"

TEST(bitstreamin, basic) { // NOLINT
    {
        std::istringstream iss("");
        BitStreamIn bsi(iss);
        EXPECT_ANY_THROW(bsi.skip(1));
    }
    {
        std::istringstream iss(" ", std::ios::binary);
        BitStreamIn bsi(iss);
        for (int i=0; i<8; ++i) {
            EXPECT_NO_THROW(bsi.skip(1));
        }
        EXPECT_ANY_THROW(bsi.skip(1));
    }

}

TEST(bitstreamin, readBool) { // NOLINT
    {
        std::istringstream iss({'\x01', '\x10'}, std::ios::binary);
        BitStreamIn bsi(iss);
        EXPECT_EQ(bsi.readBool(8), true);
        EXPECT_EQ(bsi.readBool(1), false);
        EXPECT_EQ(bsi.readBool(1), false);
        EXPECT_EQ(bsi.readBool(1), false);
        EXPECT_EQ(bsi.readBool(1), true);
        EXPECT_NO_THROW(bsi.skip(4));
        EXPECT_ANY_THROW(bsi.skip(1));
    }
}

TEST(bitstreamin, readInteger) { // NOLINT
    {
        std::istringstream iss({'\x10', '\x0f'}, std::ios::binary);
        BitStreamIn bsi(iss);
        EXPECT_EQ(bsi.readInteger<uint8_t>(8), 16);
        EXPECT_EQ(bsi.readInteger<uint8_t>(8), 15);
        EXPECT_ANY_THROW(bsi.skip(1));
    }
    {
        std::istringstream iss({'\xff', '\x01', '\x02', '\xaf', '\x73', '\xdf'}, std::ios::binary);
        BitStreamIn bsi(iss);
        EXPECT_EQ(bsi.readInteger<int16_t>(24), 258);
        EXPECT_EQ(bsi.readInteger<int16_t>(20), -2243);
        EXPECT_NO_THROW(bsi.skip(4));
    }
    {
        std::istringstream iss({'\xff', '\xf7', '\x83', '\xd9'}, std::ios::binary);
        BitStreamIn bsi(iss);
        EXPECT_NO_THROW(bsi.skip(8));
        EXPECT_EQ(bsi.readInteger<int32_t>(24), 16221145);
    }
}

TEST(bitstreamin, readShortBitSet) { // NOLINT
    {
        std::istringstream iss({'\x10', '\x0f'}, std::ios::binary);
        BitStreamIn bsi(iss);
        ShortBitSet bs = bsi.readShortBitSet(16);
        EXPECT_EQ(bs.size(), 16);
        size_t i=0;
        for (; i<3; ++i) {
            EXPECT_EQ(bs.at(i), false);
        }
        EXPECT_EQ(bs.at(i++), true);
        for (; i<8+4; ++i) {
            EXPECT_EQ(bs.at(i), false);
        }
        for (; i<16; ++i) {
            EXPECT_EQ(bs.at(i), true);
        }
        EXPECT_ANY_THROW(bsi.skip(1));
    }
    {
        std::istringstream iss({'\x00', '\x00', '\x00', '\x00', '\x10'}, std::ios::binary);
        BitStreamIn bsi(iss);
        ShortBitSet bs = bsi.readShortBitSet(36);
        bsi.skip(4);
        EXPECT_ANY_THROW(bsi.skip(1));
        EXPECT_EQ(bs.size(), 32);
        for (size_t i=0; i<31; ++i) {
            EXPECT_EQ(bs.at(i), false);
        }
        EXPECT_EQ(bs.at(31), true);
    }
}