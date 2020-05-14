@ECHO OFF
ECHO [VTX] CLEANING SOLUTION
rmdir /s /q "CMakeFiles"
rmdir /s /q ".vs"
rmdir /s /q "VTX.dir"
rmdir /s /q "x64"
rmdir /s /q "Debug"
rmdir /s /q "Release"
rmdir /s /q "RelWithDebInfo"
rmdir /s /q "build"
del /q "CMakeCache.txt"
del /q "*.cmake"
del /q "*.vcxproj"
del /q "*.sln"
del /q "*.filters"
del /q "*.user"
ECHO [VTX] DONE
PAUSE