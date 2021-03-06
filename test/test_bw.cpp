#include <gtest/gtest.h>
#include <sstream>

#include "BurrowsWheeler.h"


TEST(bw, encode) { // NOLINT
    std::string sOrig = "ABRACADABRA!";
    std::istringstream iss(sOrig);
    std::ostringstream oss;
    bw::encode(iss, oss);
    std::string sEnc = oss.str();
    EXPECT_EQ(sEnc, (std::string{0x00, 0x00, 0x00, 0x03, 'A', 'R', 'D', '!', 'R', 'C', 'A', 'A', 'A', 'A', 'B', 'B'}));
}

TEST(bw, encodeAndDecode) { // NOLINT
    auto encodeAndDecodeFun = [](const std::string& sOrig) {
        std::istringstream iss(sOrig);
        std::ostringstream oss;
        bw::encode(iss, oss);
        std::string sEnc = oss.str();

        iss = std::istringstream(sEnc);
        oss = std::ostringstream();
        bw::decode(iss, oss);
        std::string sDec = oss.str();
        EXPECT_EQ(sOrig, sDec);
    };

    encodeAndDecodeFun("ABRACADABRA!");
    encodeAndDecodeFun("CADABRA!ABRA");
    encodeAndDecodeFun("couscous");
    encodeAndDecodeFun("*************");
    encodeAndDecodeFun("foobar#§$%&/()=");
    encodeAndDecodeFun("äöü+#``?%$\"!\"§$%€");
}