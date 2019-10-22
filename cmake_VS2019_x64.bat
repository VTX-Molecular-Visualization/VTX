@ECHO OFF
ECHO [VTX] BUILDING SOLUTION FOR VS2019 x64
cmake -B "_windows" -G "Visual Studio 16 2019" -DCMAKE_GENERATOR_PLATFORM=x64 -DCMAKE_TOOLCHAIN_FILE=D:\Drive\Freelance\Projets\CNAM\vcpkg\scripts\buildsystems\vcpkg.cmake --clean-first .
ECHO [VTX] DONE
PAUSE

:: -DCMAKE_TOOLCHAIN_FILE="D:\\Drive\\Freelance\\Projets\\CNAM\\vcpkg\\scripts\\buildsystems\\vcpkg.cmake"