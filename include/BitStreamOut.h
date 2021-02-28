#ifndef STRING_PROCESSING_CPP_BITSTREAMOUT_H
#define STRING_PROCESSING_CPP_BITSTREAMOUT_H

#include <ostream>
#include <cstdint>
#include <ShortBitSet.h>

// Wrapper for easy writing of bits to std::ostream
class BitStreamOut {
public:
    explicit BitStreamOut(std::ostream& _os) : os{_os} {}
    BitStreamOut() = delete;
    BitStreamOut(BitStreamOut&& rhs) = delete;
    BitStreamOut(const BitStreamOut& rhs) = delete;
    BitStreamOut& operator=(const BitStreamOut& rhs) = delete;

    void writeEmpty(const size_t totalLength) {
        for (size_t written=0; written < totalLength; ++written) {
            ++idx;
            if (fullByte()) flush();
        }
    }

    // write integer bits (length is calculated starting at least significant bit)
    template<typename T>
    void writeInteger(const T val, const size_t totalLengthBits = 8 * sizeof(T)) {
        static_assert(std::is_integral_v<T>, "T must be integral type");
        if (totalLengthBits < 1) throw std::invalid_argument("Integer write needs at least one bit");

        constexpr size_t numBitsFull = 8*sizeof(T);
        size_t bitsToWrite = totalLengthBits;
        if (bitsToWrite > numBitsFull) {
            writeEmpty(totalLengthBits - numBitsFull);
            bitsToWrite -= numBitsFull;
        }

        while (bitsToWrite-- > 0) {
            // bitsToWrite is also offset
            curr |= ( (val & (static_cast<T>(1) << bitsToWrite)) >> bitsToWrite ) << (7 - idx);
            ++idx;
            if (fullByte()) flush();
        }
    }

    void write(const bool val, const size_t totalLengthBits = 1) {
        if (totalLengthBits < 1) throw std::invalid_argument("Bool write needs at least one bit");
        writeEmpty(totalLengthBits - 1);

        if (val) {
            curr |= 1 << (7 - idx);
        }
        ++idx;

        if (fullByte()) flush();
    }

    // write all bits of ShortBitSet
    void write(const ShortBitSet& bs) {
        for (size_t i=0; i < bs.size(); ++i) {
            if (bs.at(i)) {
                curr |= 1 << (7 - idx);
            }
            ++idx;
            if (fullByte()) flush();
        }
    }

    // force to write out current buffer (except if empty)
    void flush() {
        if (idx == 0) return;
        os.write(reinterpret_cast<char*>(&curr), 1);
        curr = 0;
        idx = 0;
    }

    ~BitStreamOut() {
        // write last char if at least one bit was set
        flush();
    }
private:
    std::ostream& os; // output stream
    uint8_t curr = 0; // buffer of length 8 bits
    size_t idx = 0; // next index in buffer that can be written

    // whether the current 8 bit buffer is full
    [[nodiscard]]
    bool fullByte() const {
        return idx == 8;
    }
};

#endif //STRING_PROCESSING_CPP_BITSTREAMOUT_H
