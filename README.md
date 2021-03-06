# Compression algorithms in C++

## `src/`
- `compress` executable that can compress and extract arbitrary files with various compression methods (default: Huffman):
   - ```
      $ build/compress -h
      Usage: build/compress [options] INPUT_FILENAME
      -h, --help
        Show this help message
      -l, --lzw
        Use LZW compression instead of Huffman
      -b, --bwmh
        Use Burrows-Wheeler, move-to-front, and then Huffman compression
        instead of only Huffman
      -x, --extract
        Extract input file instead of compressing it

      Examples:
      build/compress input.txt		Compress input.txt with Huffman
      build/compress -l input.txt		Compress input.txt with LZW
      build/compress -xl input.txt.lzw	Extract input.txt.lzw with LZW
     ```

## `include/`
- `huffman::compress` and `huffman::expand`  to apply [Huffman-style](https://en.wikipedia.org/wiki/Huffman_coding) lossless compression to data streams
- `lzw::compress` and `lzw::expand` to apply [Lempel–Ziv–Welch (LZW)](https://en.wikipedia.org/wiki/Lempel%E2%80%93Ziv%E2%80%93Welch) lossless compression to data streams
- `BitStreamIn` as a wrapper to easily read a bit stream from `std::istream`
- `BitStreamOut` as a wrapper to easily write a bit stream to `std::ostream`
- `ShortBitSet` as a simple bit set with up to 32 bits (stored in a `uint32_t`) and some convenience functions
- `bw::encode` and `bw::decode` for applying the [Burrows-Wheeler transform](https://en.wikipedia.org/wiki/Burrows%E2%80%93Wheeler_transform) (Note: not the most efficient implementation due to circular suffix array sorting)
- `mtf::encode` and `mtf::decode` for applying the [Move-to-front transform](https://en.wikipedia.org/wiki/Move-to-front_transform)

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