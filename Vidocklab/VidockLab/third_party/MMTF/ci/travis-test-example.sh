#!/bin/bash

# Test compilation of example code using C++03 if possible
# -> only expected to work in Linux or Mac with CXX set to g++ or clang++
# -> expects TRAVIS_BUILD_DIR, EMSCRIPTEN, CXX to be set

# abort on error and exit with proper exit code
set -e
# test example
cd $TRAVIS_BUILD_DIR/examples
if [ -z "$EMSCRIPTEN" ]; then
    # Compile with C++03 forced
    $CXX -I"../msgpack-c/include" -I"../include" -std=c++03 -O2 \
         -o read_and_write read_and_write.cpp
    ./read_and_write ../mmtf_spec/test-suite/mmtf/3NJW.mmtf test.mmtf
else
    # Cannot do C++03 here and need to embed input file for running it with node
    cp ../mmtf_spec/test-suite/mmtf/3NJW.mmtf .
    $CXX -I"../msgpack-c/include" -I"../include" -O2 \
         -o read_and_write.js read_and_write.cpp --embed-file 3NJW.mmtf
    node read_and_write.js 3NJW.mmtf test.mmtf
fi
