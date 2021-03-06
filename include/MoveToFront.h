#ifndef COMPRESSION_CPP_MOVETOFRONT_H
#define COMPRESSION_CPP_MOVETOFRONT_H

#include <istream>
#include <ostream>
#include <array>
#include <numeric>

namespace mtf {
    // apply move-to-front encoding
    [[maybe_unused]]
    static void encode(std::istream& is, std::ostream& os) {
        constexpr int R = 256;
        std::array<uint8_t, R> charByRank{};
        std::array<uint8_t, R> rankByChar{};
        std::iota(charByRank.begin(), charByRank.end(), 0); // fill with index
        std::iota(rankByChar.begin(), rankByChar.end(), 0); // fill with index

        uint8_t c;
        while (is.read(reinterpret_cast<char*>(&c), 1)) {
            uint8_t rankC = rankByChar[c];
            os.write(reinterpret_cast<const char*>(&rankC), 1); // write current index of c
            // shuffle to the right
            for (int i = rankC; i > 0; --i) {
                rankByChar[charByRank[i-1]]  = i;
                charByRank[i] = charByRank[i - 1];
            }
            rankByChar[c] = 0;
            charByRank[0] = c;
        }
    }

    // reverse move-to-front encoding
    [[maybe_unused]]
    static void decode(std::istream& is, std::ostream& os) {
        constexpr int R = 256;
        std::array<uint8_t, R> charByRank{};
        std::array<uint8_t, R> rankByChar{};
        std::iota(charByRank.begin(), charByRank.end(), 0); // fill with index
        std::iota(rankByChar.begin(), rankByChar.end(), 0); // fill with index

        uint8_t rankC;
        while (is.read(reinterpret_cast<char*>(&rankC), 1)) {
            uint8_t c = charByRank[rankC];
            os.write(reinterpret_cast<const char*>(&c), 1); // write decoded char

            // shuffle to the right
            for (int i=rankC; i > 0; --i) {
                charByRank[i] = charByRank[i-1];
                rankByChar[charByRank[i-1]] = i;
            }
            charByRank[0] = c;
            rankByChar[c] = 0;
        }
    }
}

#endif //COMPRESSION_CPP_MOVETOFRONT_H
