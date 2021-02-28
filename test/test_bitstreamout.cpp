#include <gtest/gtest.h>
#include <stack>
#include "BitStreamOut.h"

TEST(bitstreamout, writeBool) { // NOLINT
    std::ostringstream ss;

    {
        BitStreamOut bso(ss);
        bso.write(true);
        bso.write(true);
        bso.write(true);
        bso.write(false);
        bso.write(true);
        bso.write(true);
        bso.write(true);
        bso.write(true);

        bso.write(true);
        bso.writeEmpty(8);
    }

    {
        std::string res = ss.str();
        EXPECT_EQ(res.size(), 3);
        EXPECT_EQ(static_cast<uint8_t>(res[0]), 0b11101111);
        EXPECT_EQ(static_cast<uint8_t>(res[1]), 0b10000000);
        EXPECT_EQ(static_cast<uint8_t>(res[2]), 0b00000000);
    }

}

TEST(bitstreamout, writeInteger) { // NOLINT
    std::ostringstream ss;
    {
        BitStreamOut bso(ss);
        bso.writeInteger(static_cast<int32_t>(64+7), 3);
        bso.flush();
        bso.writeInteger(static_cast<int32_t>(-7), 3); // should write 0b001
        bso.flush();
        bso.writeInteger(static_cast<uint64_t>(std::numeric_limits<uint64_t>::max()), 64); // 8 bytes of 0xff

    }
    {
        std::string res = ss.str();
        EXPECT_EQ(res.size(), 10);
        EXPECT_EQ(static_cast<uint8_t>(res[0]), 0b11100000);
        EXPECT_EQ(static_cast<uint8_t>(res[1]), 0b00100000);
        for (size_t i=2; i<2+8; ++i) {
            EXPECT_EQ(static_cast<uint8_t>(res[i]), 0xff);
        }
    }
}

TEST(bitstreamout, writeShortBitSet) { // NOLINT
    ShortBitSet bs;
    bs.push_back(true);
    bs.push_back(true);
    bs.push_back(true);
    bs.push_back(false);
    bs.push_back(false);
    bs.push_back(true);
    // 0b111001
    std::ostringstream ss;
    {
        BitStreamOut bso(ss);
        bso.write(bs);
    }
    {
        std::string res = ss.str();
        EXPECT_EQ(res.size(), 1);
        EXPECT_EQ(static_cast<uint8_t>(res[0]), 0b11100100);
    }

    bs.push_back(false);
    bs.push_back(true);

    bs.push_back(true);
    bs.push_back(true);

    ss = std::ostringstream();
    {
        BitStreamOut bso(ss);
        bso.write(bs);
    }
    {
        std::string res = ss.str();
        EXPECT_EQ(res.size(), 2);
        EXPECT_EQ(static_cast<uint8_t>(res[0]), 0b11100101);
        EXPECT_EQ(static_cast<uint8_t>(res[1]), 0b11000000);
    }
}