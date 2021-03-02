#!/bin/bash
set -eux

EXECUTABLE=$1
FILE=$2

echo "Testing compress executable..."
echo "Executing in: $(pwd)"
echo "Compressing the file $FILE"

# test huffman
$EXECUTABLE $FILE
$EXECUTABLE -x $FILE".huffman"
cmp $FILE $FILE".huffman.orig"
# test lzw
$EXECUTABLE -l $FILE
$EXECUTABLE -xl $FILE".lzw"
cmp $FILE $FILE".lzw.orig"
