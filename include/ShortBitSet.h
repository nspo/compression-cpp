#ifndef STRING_PROCESSING_CPP_SHORTBITSET_H
#define STRING_PROCESSING_CPP_SHORTBITSET_H

// Bit set that can store 0 to 32 bits
class ShortBitSet {
public:
    constexpr static size_t max_size = 32;

    // append bit as LSB/to the right and push other values to the left
    void push_back(const bool val) {
        if (numBits == ShortBitSet::max_size) throw std::runtime_error("ShortBitSet is already full");

        data <<= 1;
        data |= val;
        ++numBits;
    }

    // pop bit from LSB/right end and move other values to the right
    bool pop_back() {
        if (numBits == 0) throw std::runtime_error("ShortBitSet is already empty");

        const bool val = data & 1;
        data >>= 1;
        --numBits;

        return val;
    }

    // return bit value at index, with index=0 being at left end/MSB
    [[nodiscard]]
    bool at(const uint8_t index) const {
        if (index >= numBits) throw std::runtime_error("Invalid index");
        return data & (1 << (numBits -1 - index));
    }

    [[nodiscard]]
    uint8_t size() const {
        return numBits;
    }

    [[nodiscard]]
    bool empty() const {
        return numBits == 0;
    }

    void clear() {
        data = 0;
        numBits = 0;
    }
private:
    uint32_t data = 0;
    uint8_t numBits = 0;
};

#endif //STRING_PROCESSING_CPP_SHORTBITSET_H
