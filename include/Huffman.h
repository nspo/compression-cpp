#ifndef STRING_PROCESSING_CPP_HUFFMAN_H
#define STRING_PROCESSING_CPP_HUFFMAN_H

#include <memory>
#include <queue>
#include <functional>
#include <sstream>
#include <iostream>
#include "PriorityQueueExtended.h" // priority_queue which works with unique_ptr
#include "ShortBitSet.h"
#include "BitStreamOut.h"
#include "BitStreamIn.h"

namespace huffman {

    class Node;
    using NodePtr = std::unique_ptr<Node>;

    const int R = 256; // extended ASCII radix
    using TrieTable = std::array<ShortBitSet, R>;

    // A node in the trie, representing a single character (leaf) or sub-trie
    class Node {
    public:
        explicit Node(char ch, int freq, NodePtr left, NodePtr right) :
                m_ch{ch}, m_freq{freq}, m_left{std::move(left)}, m_right{std::move(right)} {}

        [[nodiscard]]
        bool isLeaf() const {
            return !m_left && !m_right;
        }

        [[nodiscard]]
        char ch() const { return m_ch; }

        [[nodiscard]]
        int freq() const { return m_freq; }

        [[nodiscard]]
        Node *left() const { return m_left.get(); }

        [[nodiscard]]
        Node *right() const { return m_right.get(); }

    private:
        char m_ch;
        int m_freq;
        NodePtr m_left;
        NodePtr m_right;
    };

    // Comparison operator (to be able to use NodePtr in priority queue)
    bool operator>(const NodePtr &lhs, const NodePtr &rhs) {
        return lhs->freq() > rhs->freq();
    }

    // check (sub-)tries for equality (used in unit tests)
    bool operator==(const Node &lhs, const Node &rhs) {
        if (&lhs == &rhs) return true;
        if (lhs.isLeaf() != rhs.isLeaf()) return false;
        if (lhs.isLeaf()) {
            // both are leaves
            return lhs.ch() == rhs.ch();
        }
        if (lhs.left() == nullptr || lhs.right() == nullptr || rhs.left() == nullptr || rhs.right() == nullptr) {
            // invalid
            return false;
        }
        // recurse
        return *lhs.left() == *rhs.left() && *lhs.right() == *rhs.right();
    }

    namespace internal {
        // Build a trie using the number of occurrences of each character
        NodePtr buildTrie(const std::array<int, R> &frequencies) {
            priority_queue<NodePtr, std::vector<NodePtr>, std::greater<>> minPQ;

            // Create nodes for all characters
            for (int i = 0; i < R; ++i) {
                const char c = static_cast<char>(i);
                if (frequencies[i] > 0) {
                    minPQ.push(std::make_unique<Node>(c, frequencies[i], NodePtr{}, NodePtr{}));
                }
            }

            // repeatedly combine the two least occurring subtrees
            while (minPQ.size() > 1) {
                NodePtr left = minPQ.pop_top();
                NodePtr right = minPQ.pop_top();

                NodePtr parent = std::make_unique<Node>('\0', left->freq() + right->freq(), std::move(left),
                                                        std::move(right));
                minPQ.push(std::move(parent));
            }

            if (minPQ.empty()) {
                // no values found in input
                return {};
            }
            return minPQ.pop_top(); // return root
        }

        void trie2table(const huffman::Node &node, ShortBitSet &stack, TrieTable &result) {
            if (node.isLeaf()) {
                result[static_cast<uint8_t>(node.ch())] = stack; // do not use char as it's signed!
            } else {
                if (node.left()) {
                    stack.push_back(false);
                    trie2table(*node.left(), stack, result);
                    stack.pop_back();
                }
                if (node.right()) {
                    stack.push_back(true);
                    trie2table(*node.right(), stack, result);
                    stack.pop_back();
                }
            }
        }


        // Generate table of (encoded) values for each character that occurs, empty strings for non-occurring characters
        TrieTable trie2table(const huffman::Node &node) {
            TrieTable table{};
            ShortBitSet stack;
            internal::trie2table(node, stack, table);
            return table;
        }


        void printTrie(const huffman::Node &node, ShortBitSet &stack) {
            if (node.isLeaf()) {
                for (size_t i = 0; i < stack.size(); ++i) {
                    std::cout << stack.at(i);
                }

                std::cout << ": ";
                // consider newline and carriage return as special
                if (node.ch() == '\n') {
                    std::cout << "LF";
                } else if (node.ch() == '\r') {
                    std::cout << "CR";
                } else {
                    std::cout << node.ch();
                }
                std::cout <<  "/" << static_cast<int>(node.ch()) << " has freq " << node.freq()
                          << "\n";
            }

            if (node.left()) {
                stack.push_back(false);
                printTrie(*node.left(), stack);
                stack.pop_back();
            }

            if (node.right()) {
                stack.push_back(true);
                printTrie(*node.right(), stack);
                stack.pop_back();
            }
        }


        void printTrie(const huffman::Node &node) {
            ShortBitSet stack;
            internal::printTrie(node, stack);
        }


        // convert bit input stream to trie
        NodePtr readTrie(BitStreamIn &bsi) {
            if (bsi.readBool()) {
                // a leaf follows
                char c = bsi.readInteger<char>(8);
                return std::make_unique<Node>(c, 0, NodePtr{}, NodePtr{});
            }
            NodePtr x = readTrie(bsi);
            NodePtr y = readTrie(bsi);
            return std::make_unique<Node>('\0', 0, std::move(x), std::move(y));
        }

        // write trie to output bit stream
        void writeTrie(BitStreamOut &bso, const Node &node) {
            if (node.isLeaf()) {
                bso.write(true);
                bso.writeInteger(node.ch());
                return;
            }
            // not a leaf
            bso.write(false);
            writeTrie(bso, *node.left());
            writeTrie(bso, *node.right());
        }

        void printTable(const TrieTable &table) {
            for (size_t i = 0; i < R; ++i) {
                if (!table[i].empty()) {
                    std::cout << static_cast<char>(i) << ": ";
                    for (size_t j = 0; j < table[i].size(); ++j) {
                        std::cout << table[i].at(j);
                    }
                    std::cout << "\n";
                }
            }
        }

    }

    // build a trie for the data in input stream
    // @param readBytes optionally return the number of bytes read from input stream
    NodePtr buildTrie(std::istream &is, std::optional<std::reference_wrapper<uint32_t>> readBytes = {}) {
        std::array<int, R> freq{};
        uint8_t c;
        uint32_t numReadBytes = 0;
        while (is.read(reinterpret_cast<char*>(&c), 1)) {
            ++freq[c];
            ++numReadBytes;
        }
        if (readBytes) {
            readBytes->get() = numReadBytes; // it's tricky to actually set the referenced value and not just rebind
        }
        return internal::buildTrie(freq);
    }

    // build a trie for the data in string_view
    NodePtr buildTrie(std::string_view sv) {
        std::array<int, R> freq{};
        for(char c : sv) {
            ++freq[c];
        }
        return internal::buildTrie(freq);
    }

    namespace internal {
        // decompress input bit stream to output bit stream
        void expand(BitStreamIn &input, BitStreamOut &output) {
            const NodePtr root = internal::readTrie(input);

            const auto N = input.readInteger<uint32_t>();
            for (size_t i = 0; i < N; ++i) {
                const Node *node = root.get();
                while (!node->isLeaf()) {
                    if (!input.readBool()) {
                        node = node->left();
                    } else {
                        node = node->right();
                    }
                }
                output.writeInteger<char>(node->ch());
            }
        }

        // compress inputSize bytes of input into output using an already generated trie
        void compress(std::istream &input, const uint32_t inputSize, BitStreamOut &output, const Node &trieRoot) {
            internal::writeTrie(output, trieRoot);
            output.writeInteger(inputSize);
            const TrieTable table = internal::trie2table(trieRoot);

            for (size_t i = 0; i < inputSize; ++i) {
                uint8_t c;
                if (!input.read(reinterpret_cast<char*>(&c), 1)) {
                    throw std::runtime_error("Input ended unexpectedly");
                }
                if (table[c].empty()) {
                    throw std::runtime_error("No code for input char found in trie");
                }
                output.write(table[c]);
            }
            output.flush();
        }

        // compress the input, given as two independent streams to the same data, into output
        void compress(std::istream &input1, std::istream &input2, BitStreamOut &output) {
            if (&input1 == &input2) throw std::runtime_error("input1 and input2 may not be the same object");

            uint32_t inputSize = 0;
            const auto trieRoot = huffman::buildTrie(input1, std::ref(inputSize));

            compress(input2, inputSize, output, *trieRoot);
        }
    }


    // expend encoded input stream into output
    void expand(std::istream& is, std::ostream& os) {
        BitStreamIn bsi(is);
        BitStreamOut bso(os);
        internal::expand(bsi, bso);
    }

    // decompress encoded input string
    std::string expand(const std::string &inputCompressed) {
        std::istringstream issComp = std::istringstream(inputCompressed, std::ios::binary);
        BitStreamIn bsiComp(issComp);

        std::ostringstream oss = std::ostringstream(std::ios::binary);

        BitStreamOut bsoDec(oss);
        internal::expand(bsiComp, bsoDec);
        bsoDec.flush();

        return oss.str();
    }

    // compress string_view into output string
    std::string compress(const std::string &input) {
        std::istringstream iss(input, std::ios::binary);
        const auto trieRoot = buildTrie(iss);

        // read again for compression
        iss = std::istringstream(input, std::ios::binary);

        std::ostringstream oss(std::ios::binary);
        BitStreamOut bso(oss);
        internal::compress(iss, input.size(), bso, *trieRoot);

        return oss.str();
    }

    // compress the input, given as two independent streams to the same data, into output
    void compress(std::istream &input1, std::istream &input2, std::ostream& output) {
        BitStreamOut bso(output);
        internal::compress(input1, input2, bso);
    }


}

#endif //STRING_PROCESSING_CPP_HUFFMAN_H
