#ifndef COMPRESSION_CPP_BURROWSWHEELER_H
#define COMPRESSION_CPP_BURROWSWHEELER_H

#include <istream>
#include <ostream>
#include <sstream>
#include "BitStreamIn.h"
#include "CircularSuffix.h"
#include "BitStreamOut.h"

namespace bw {
    [[maybe_unused]]
    static void encode(std::istream& is, std::ostream& os) {
        std::basic_stringstream<uint8_t> ss;
        uint8_t c;
        while (is.read(reinterpret_cast<char*>(&c), 1)) {
            ss.write(&c, 1);
        }
        const std::basic_string<uint8_t> s = ss.str();
        const auto order = circular_suffix::sort<uint8_t>(s); // this can be implemented more efficiently
        // write index of original string in sorted suffix array
        for (size_t i=0; i < s.size(); ++i) {
            if (order[i] == 0) {
                BitStreamOut bso(os);
                bso.writeInteger(i, 32);
                break;
            }
        }

        for (const auto startIndex : order) {
            const size_t indexLastCol = (startIndex + s.size() - 1) % s.size();
            os.write(reinterpret_cast<const char*>(&s[indexLastCol]), 1);
        }
    }

    [[maybe_unused]]
    static void decode(std::istream& is, std::ostream& os) {
        uint32_t first;
        {   // limit scope of bsi
            BitStreamIn bsi(is);
            first = bsi.readInteger<uint32_t>();
        }
        std::basic_stringstream<uint8_t> ss;
        {
            uint8_t c;
            while (is.read(reinterpret_cast<char*>(&c), 1)) {
                ss.write(&c, 1);
            }
        }
        const std::basic_string<uint8_t> sLastCol = ss.str();

        // count occurrences of each char
        constexpr size_t R = 256;
        std::array<size_t,R+1> count{}; // offset of +1 while counting
        for (const size_t c : sLastCol) {
            ++count[c+1];
        }

        // accumulate count
        for (size_t i=1; i < count.size(); ++i) {
            count[i] += count[i-1];
        }

        // determine sorted string (first column of sorted suffix arrays) and the next index to look at for each
        std::vector<size_t> next(sLastCol.size());
        std::basic_string<uint8_t> sFirstCol(sLastCol.size(), 0);
        for (size_t i=0; i < sLastCol.size(); ++i) {
            const size_t sortedI = count[sLastCol[i]]++; // index of sLastCol[i] after sorting
            sFirstCol[sortedI] = sLastCol[i];
            next[sortedI] = i;
        }

        // write result
        size_t index = first;
        for (size_t i=0; i < sLastCol.size(); ++i) {
            os.write(reinterpret_cast<const char*>(&sFirstCol[index]), 1);
            index = next[index];
        }
    }
} // bw

#endif //COMPRESSION_CPP_BURROWSWHEELER_H
