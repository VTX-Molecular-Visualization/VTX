#!/bin/bash

export CMAKE_ARGS="-DCMAKE_BUILD_TYPE=debug -DBUILD_TESTS=ON -Dmmtf_build_local=ON"

if [[ "$EMSCRIPTEN" == "ON" ]]; then
    # Install a Travis compatible emscripten SDK
    wget https://github.com/chemfiles/emscripten-sdk/archive/master.tar.gz
    tar xf master.tar.gz
    ./emscripten-sdk-master/emsdk activate
    source ./emscripten-sdk-master/emsdk_env.sh

    export CMAKE_CONFIGURE='emcmake'
    export CMAKE_ARGS="$CMAKE_ARGS -DTEST_RUNNER=node -DCMAKE_BUILD_TYPE=release"

    # Install a modern cmake
    cd $HOME
    wget https://cmake.org/files/v3.9/cmake-3.9.3-Linux-x86_64.tar.gz
    tar xf cmake-3.9.3-Linux-x86_64.tar.gz
    export PATH=$HOME/cmake-3.9.3-Linux-x86_64/bin:$PATH

    export CC=emcc
    export CXX=em++

    return
fi

if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
    if [[ "$CC" == "gcc" ]]; then
        export CC=gcc-4.8
        export CXX=g++-4.8
    fi
fi

if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
    if [[ "$CC" == "gcc" ]]; then
        export CC=gcc-6
        export CXX=g++-6
    fi
fi

if [[ "$ARCH" == "x86" ]]; then
    export CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_CXX_FLAGS=-m32 -DCMAKE_C_FLAGS=-m32"
fi
