#include <gtest/gtest.h>
#include <stack>
#include <fstream>
#include "LZW.h"
#include <string>


TEST(lzw, basic) { // NOLINT
    std::string sRef = "ABRACADABRABRABRABRABRABRABRA";
    std::string sCompressed;
    {
        std::istringstream iss(sRef);
        std::ostringstream oss;
        lzw::compress(iss, oss);
        sCompressed = oss.str();
    }

    std::string sDecompressed;
    {
        std::istringstream iss(sCompressed);
        std::ostringstream oss;
        lzw::expand(iss, oss);
        sDecompressed = oss.str();
    }
    EXPECT_EQ(sRef, sDecompressed);

//    auto print_as_hex = [](const std::string& s) {
//        std::cout << "0x" << std::hex;
//        for (char c : s) {
//            std::cout << static_cast<int>(c);
//        }
//        std::cout << std::dec << " (" << s.size() << " bytes)\n";
//    };
//    std::cout << "Input       : " << sRef << " (" << sRef.size() << " bytes)\n";
//    std::cout << "Input       : ";
//    print_as_hex(sRef);
//
//    std::cout << "Compressed  : ";
//    print_as_hex(sCompressed);
//
//    std::cout << "Decompressed: " << sDecompressed << "\n";
}
