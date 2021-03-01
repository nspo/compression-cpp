# Compression algorithms in C++

## `include/`
- `huffman::compress` and `huffman::expand`  to apply [Huffman-style](https://en.wikipedia.org/wiki/Huffman_coding) lossless compression to data streams
- `lzw::compress` and `lzw::expand` to apply [Lempel–Ziv–Welch (LZW)](https://en.wikipedia.org/wiki/Lempel%E2%80%93Ziv%E2%80%93Welch) lossless compression to data streams
- `BitStreamIn` as a wrapper to easily read a bit stream from `std::istream`
- `BitStreamOut` as a wrapper to easily write a bit stream to `std::ostream`
- `ShortBitSet` as a simple bit set with up to 32 bits (stored in a `uint32_t`) and some convenience functions

## Compilation and execution
- Download submodules (for unit tests): `git submodule update --init --recursive`
- Compile with `cmake`:
  ```bash
  mkdir build
  cd build/
  cmake ..
  make
  ```
- Go to top-level folder again: `cd ..`
- Run all tests: `find build/ -name "*_gtest" -exec {} \;`

## References
- Introduction to Algorithms by Cormen et al.
- Algorithms, [Part II](https://www.coursera.org/learn/algorithms-part2/home/welcome) by Princeton University (Robert Sedgewick, Kevin Wayne)