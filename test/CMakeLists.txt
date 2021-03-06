set(BINARY compression_gtest)

add_executable(${BINARY}
                test_huffman.cpp
                test_shortbitset.cpp
                test_bitstreamout.cpp
                test_bitstreamin.cpp
                test_lzw.cpp
                test_mtf.cpp
                test_cs.cpp
                test_bw.cpp
)

add_test(NAME ${BINARY} COMMAND ${BINARY})

target_link_libraries(${BINARY} PUBLIC gtest gtest_main pthread)
