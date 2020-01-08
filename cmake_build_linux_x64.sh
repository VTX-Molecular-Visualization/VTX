#!/bin/sh
echo [VTX] BUILDING SOLUTION FOR LINUX x64
cmake -B "_linux" -DCMAKE_GENERATOR_PLATFORM=x64 -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake --clean-first .
echo [VTX] DONE
