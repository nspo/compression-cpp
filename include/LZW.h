#ifndef STRING_PROCESSING_CPP_LZW_H
#define STRING_PROCESSING_CPP_LZW_H

#include <istream>
#include <ostream>
#include "BitStreamOut.h"
#include "BitStreamIn.h"
#include "external/TernarySearchTrie.h"

namespace lzw {
    constexpr static int R = 256; // number of distinct inputs (8 bits each)
    constexpr static int L = 4096; // number of codewords, 2^12
    constexpr static int W = 12; // length in bit of codewords

    [[maybe_unused]]
    static void compress(std::istream &is, std::ostream &os) {
        // read complete input into string
        const std::string stringInput(std::istreambuf_iterator<char>(is), {});
        std::string_view input = stringInput;

        // create wrapper for binary writing to ostream
        BitStreamOut bso(os);
        TernarySearchTrie<int> st;
        // fill in codewords for "normal" characters
        for (int i = 0; i < R; ++i) {
            st.put(std::string(1, static_cast<char>(i)), i);
        }

        int code = R + 1; // next codeword we can set - reserve R for end-of-file/EOF
        while (!input.empty()) {
            std::string_view svLongestPrefix = st.longestPrefixOf(input);
            bso.writeInteger(*st.get(svLongestPrefix), W); // write encoded form
            size_t t = svLongestPrefix.size();
            if (t < input.size() && code < L) {
                // save new codeword
                st.put(input.substr(0, t + 1), code++);
            }
            // shorten remaining input
            input = input.substr(t);
        }
        bso.writeInteger(R, W); // write EOF
        bso.flush();
    }

    [[maybe_unused]]
    static void expand(std::istream &is, std::ostream &os) {
        BitStreamIn bsi(is);

        std::array<std::string,L> st; // symbol table for lookup
        int i; // next available codeword value
        for (i=0; i < R; ++i) {
            st[i] = std::string(1, static_cast<char>(i));
        }
        st[i++] = " "; // unused, EOF
        int codeword = bsi.readInteger<int>(W);
        std::string val = st[codeword];
        while (true) {
            os.write(val.c_str(), val.size()); // write current data
            codeword = bsi.readInteger<int>(W);
            if (codeword == R) {
                // EOF
                break;
            }
            std::string s = st[codeword];
            if (i == codeword) {
                // special case of invalid lookahead - make codeword from last one
                s = val + val[0];
            }
            if (i < L) {
                // add new entry to table
                st[i++] = val + s[0];
            }
            val = std::move(s);
        }
    }


} // lzw
#endif //STRING_PROCESSING_CPP_LZW_H
