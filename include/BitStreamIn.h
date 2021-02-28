#ifndef STRING_PROCESSING_CPP_BITSTREAMIN_H
#define STRING_PROCESSING_CPP_BITSTREAMIN_H

#include <istream>
#include <cstdint>
#include <ShortBitSet.h>

// Wrapper for easy reading of bits from std::istream
class BitStreamIn {
public:
    explicit BitStreamIn(std::istream& _is) : is{_is} {}
    BitStreamIn() = delete;
    BitStreamIn(BitStreamIn&& rhs) = delete;
    BitStreamIn(const BitStreamIn& rhs) = delete;
    BitStreamIn& operator=(const BitStreamIn& rhs) = delete;

    void skip(const size_t numBits) {
        for (size_t skipped=0; skipped < numBits; ++skipped) {
            if (fullByte()) {
                advance();
                checkFinished();
            }
            ++idx;
        }
    }

    // read integer (length is calculated starting at least significant bit)
    template<typename T>
    T readInteger(const size_t totalLengthBits = 8 * sizeof(T)) {
        static_assert(std::is_integral_v<T>, "T must be integral type");
        if (totalLengthBits < 1) throw std::invalid_argument("Integer read needs at least one bit");

        constexpr size_t numBitsFull = 8*sizeof(T);
        size_t bitsToRead = totalLengthBits;
        if (bitsToRead > numBitsFull) {
            size_t numSkip = totalLengthBits - numBitsFull;
            skip(numSkip);
            bitsToRead -= numSkip;
        }

        T val = 0;
        while (bitsToRead-- > 0) {
            // bitsToRead is also offset
            if (fullByte()) {
                advance();
                checkFinished();
            }
            ++idx;
            val |= static_cast<T>(currentBool() << bitsToRead);
        }

        return val;
    }

    // read a boolean value
    [[nodiscard]]
    bool readBool(const size_t totalLengthBits = 1) {
        if (totalLengthBits < 1) throw std::invalid_argument("Bool read needs at least one bit");
        skip(totalLengthBits - 1);

        if (fullByte()) {
            advance();
            checkFinished();
        }
        ++idx;
        bool val = currentBool();

        return val;
    }

    // read ShortBitSet
    ShortBitSet readShortBitSet(const size_t totalLengthBits = 1) {
        size_t bitsToRead = totalLengthBits;
        if (bitsToRead > ShortBitSet::max_size) {
            size_t numSkip = totalLengthBits - ShortBitSet::max_size;
            skip(numSkip);
            bitsToRead -= numSkip;
        }

        ShortBitSet bs;
        while (bitsToRead-- > 0) {
            // bitsToRead is also offset
            if (fullByte()) {
                advance();
                checkFinished();
            }
            ++idx;
            bs.push_back(currentBool());
        }

        return bs;
    }

    ~BitStreamIn() = default;
private:
    std::istream& is; // input stream
    uint8_t curr = 0; // last byte that was read (except if none were read yet)
    size_t idx = 7; // idx%8 is the last index in curr that was read (except if nothing was read yet)
    bool finished = false;

    // try to read next byte in stream
    void advance() {
        if (finished) throw std::runtime_error("Cannot read if already finished");
        if (idx%8 != 7) throw std::invalid_argument("Cannot advance if current buffer was not fully used yet");

        char val;
        if (is.read(&val, 1)) {
            curr = val;
        } else {
            finished = true;
        }
    }

    // throw an error if stream is finished
    void checkFinished() const {
        if (finished) {
            throw std::runtime_error("Stream finished unexpectedly");
        }
    }

    // whether the current 8 bit buffer was fully used
    [[nodiscard]]
    bool fullByte() const {
        return idx%8 == 7;
    }

    // return value of current/last read byte
    [[nodiscard]]
    bool currentBool() const {
        return curr & (1 << (7-(idx%8)));
    }
};

#endif //STRING_PROCESSING_CPP_BITSTREAMIN_H
