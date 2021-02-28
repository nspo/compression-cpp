#include <gtest/gtest.h>
#include <stack>
#include <fstream>
#include "Huffman.h"
#include <string>


TEST(huffman, writeAndReadTrie) { // NOLINT
    const std::string foo = "Lorem ipsum dolor sit amet";
    const huffman::NodePtr trieRoot1 = huffman::buildTrie(foo);

    // separate scopes to force flush
    {
        std::ofstream ofs("writeAndReadTrie.dat");
        BitStreamOut bso(ofs);
        huffman::internal::writeTrie(bso, *trieRoot1);
    }
    {
        // compare generated data with data written and read from file
        std::ifstream ifs("writeAndReadTrie.dat");
        BitStreamIn bsi(ifs);
        auto trieRoot2 = huffman::internal::readTrie(bsi);
        EXPECT_TRUE(*trieRoot1 == *trieRoot2);
    }
    {
        // compare generated data with reference data
        std::ostringstream oss;
        {
            BitStreamOut bso(oss);
            huffman::internal::writeTrie(bso, *trieRoot1);
        }
        const std::string data_actual = oss.str();
        const std::string data_ref = "\x17\x2b\x9a\xdf\x6d\x0b\x65\xd5\x64\xb8\x2e\x8a\x65\x85\x20\x59\x6d\x20";
        EXPECT_EQ(data_actual, data_ref);
    }
}

TEST(huffman, compressAndExpand) { // NOLINT
    const std::string sRef = "ABRACADABRA!ABRACADABRA!";
    const std::string sComp = huffman::compress(sRef);
    const std::string sDecomp = huffman::expand(sComp);

//    std::cout << "Original    : " << sRef << "\n";
//    std::cout << "              ^- " << sRef.size() << " bytes\n";
//    std::cout << "Compressed  : " << sComp << "\n";
//    std::cout << "              ^- " << sComp.size() << " bytes\n";
//    std::cout << "Decompressed: " << sDecomp<< "\n";
//    std::cout << "              ^- " << sDecomp.size() << " bytes\n";
    EXPECT_EQ(sDecomp, sRef);
}

TEST(huffman, easyCompressAndExpand) { // NOLINT
    const std::string stringInput = "Lorem ipsum.\r\nDolor sit amet.";

    std::istringstream iss1(stringInput);
    std::istringstream iss2(stringInput);
    std::ostringstream oss;
    huffman::compress(iss1, iss2, oss);

    const std::string stringComp = oss.str();

    std::istringstream iss3(stringComp);
    std::ostringstream oss3;
    huffman::expand(iss3, oss3);
    std::string stringDecomp = oss3.str();
    EXPECT_EQ(stringDecomp, stringInput);
}