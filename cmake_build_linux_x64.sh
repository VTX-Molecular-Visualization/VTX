#!/bin/sh
echo [VTX] CMAKE
cmake -B "_linux" -D CMAKE_C_COMPILER="/bin/gcc" -D CMAKE_CXX_COMPILER "/bin/g++" -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake --clean-first .
echo [VTX] DONE
