// from other project: https://github.com/nspo/string-processing-cpp/blob/master/include/TernarySearchTrie.h

#ifndef STRING_PROCESSING_CPP_TERNARYSEARCHTRIE_H
#define STRING_PROCESSING_CPP_TERNARYSEARCHTRIE_H

#include <optional>
#include <memory>
#include <string_view>

template<typename Value>
class TernarySearchTrie {
private:
    struct Node;
    using NodeUPtr = std::unique_ptr<Node>;
    struct Node {
        std::optional<Value> value;
        char c = '\0';
        NodeUPtr left, mid, right;
    };

    NodeUPtr root;

    NodeUPtr put(NodeUPtr x, const std::string_view key, const size_t depth, const Value value) {
        const char c = key[depth];
        if (!x) {
            x = std::make_unique<Node>();
            x->c = c;
        }
        if (c < x->c) {
            x->left = put(std::move(x->left), key, depth, value);
        } else if (c > x->c) {
            x->right = put(std::move(x->right), key, depth, value);
        } else if (depth < key.size()-1) {
            x->mid = put(std::move(x->mid), key, depth+1, value);
        } else {
            x->value = value;
        }
        return x;
    }

    Node* get(Node* x, const std::string_view key, const size_t depth) const {
        if (!x) return x;
        const char c = key[depth];
        if (c < x->c) {
            return get(x->left.get(), key, depth);
        } else if (c > x->c) {
            return get(x->right.get(), key, depth);
        } else if (depth < key.size()-1) {
            return get(x->mid.get(), key, depth+1);
        } else {
            return x;
        }
    }

    int search(Node* x, std::string_view key, const size_t depth, int length) const {
        if (!x) return length;
        if (depth == key.size()) return length;
        const char c = key[depth];

        if (c < x->c) {
            return search(x->left.get(), key, depth, length);
        } else if (c > x->c) {
            return search(x->right.get(), key, depth, length);
        } else {
            // character matches! go down middle
            if (x->value) length = depth+1;
            return search(x->mid.get(), key, depth+1, length);
        }
    }
public:
    void put(const std::string_view key, const Value value) {
        root = put(std::move(root), key, 0, value);
    }

    [[nodiscard]]
    std::optional<Value> get(const std::string_view key) const {
        Node* x = get(root.get(), key, 0);
        if (!x) return {};
        return x->value;
    }

    [[nodiscard]]
    bool contains(const std::string_view key) const {
        return get(key).has_value();
    }

    [[nodiscard]]
    std::string_view longestPrefixOf(std::string_view key) const {
        const int length = search(root.get(), key,0, 0);
        return key.substr(0, length);
    }
};

#endif //STRING_PROCESSING_CPP_TERNARYSEARCHTRIE_H
