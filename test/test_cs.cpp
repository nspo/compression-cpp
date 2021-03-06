#include <gtest/gtest.h>
#include <sstream>

#include "CircularSuffix.h"


TEST(cs, basic) { // NOLINT
    EXPECT_EQ(circular_suffix::sort<char>("ABRACADABRA!"), (std::vector<size_t>{11, 10, 7, 0, 3, 5, 8, 1, 4, 6, 9, 2}));
    EXPECT_EQ(circular_suffix::sort<char>("Z"), (std::vector<size_t>{0}));
    EXPECT_EQ(circular_suffix::sort<char>(""), (std::vector<size_t>{}));
    EXPECT_EQ(circular_suffix::sort<uint8_t>(std::basic_string<uint8_t>{0, 70, 30}),
              (std::vector<size_t>{0, 2, 1}));
}