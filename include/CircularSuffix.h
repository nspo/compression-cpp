#ifndef COMPRESSION_CPP_CIRCULARSUFFIX_H
#define COMPRESSION_CPP_CIRCULARSUFFIX_H

#include <numeric>

namespace circular_suffix {
    // sort the circular suffixes starting at each index of the input string
    // NOTE: this implementation is not very efficient! O(n^2*log(n)) in the worst case
    // @return vector of starting positions of sorted suffix array, i.e. if input is "abcd" then 0 represents "abcd",
    //         1, represents "bcda", etc.
    template<typename CharType>
    static std::vector<size_t> sort(const std::basic_string_view<CharType> sv) {
        std::vector<size_t> order(sv.size());
        std::iota(order.begin(), order.end(), 0); // fill with index

        // define closure that can be used for comparing the cyclic suffixes starting at specific indices
        auto suffComp = [sv] (const size_t lhsStart, const size_t rhsStart) {
            for (size_t offset=0; offset < sv.size(); ++offset) {
                size_t idxLhs = (lhsStart + offset) % sv.size();
                size_t idxRhs = (rhsStart + offset) % sv.size();
                if (sv[idxLhs] != sv[idxRhs]) {
                    return sv[idxLhs] < sv[idxRhs];
                }
            }
            return false; // equal
        };

        std::sort(order.begin(), order.end(), suffComp);
        return order;
    }
} // cs

#endif //COMPRESSION_CPP_CIRCULARSUFFIX_H
