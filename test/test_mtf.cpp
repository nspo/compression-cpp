#include <gtest/gtest.h>
#include <sstream>

#include "MoveToFront.h"


TEST(mtf, basic) { // NOLINT
    std::string sOrig = "ABRACADABRA!";
    std::istringstream iss(sOrig);
    std::ostringstream oss;
    mtf::encode(iss, oss);
    std::string sEnc = oss.str();

    EXPECT_EQ(sEnc, (std::string {0x41, 0x42, 0x52, 0x02, 0x44, 0x01, 0x45, 0x01, 0x04, 0x04, 0x02, 0x26}));

    iss = std::istringstream(sEnc);
    oss = std::ostringstream();
    mtf::decode(iss, oss);
    std::string sDec = oss.str();
    EXPECT_EQ(sOrig, sDec);
}