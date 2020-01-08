#!/bin/sh
echo [VTX] CMAKE
cmake -B "_linux" -DCMAKE_C_COMPILER=/bin/gcc -DCMAKE_CXX_COMPILER=/bin/g++ -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake --clean-first .
echo [VTX] DONE
