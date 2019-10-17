@ECHO OFF
ECHO [VTX] CLEANING SOLUTION
rmdir /s /q "CMakeFiles"
del /q "CMakeCache.txt"
del /q "*.cmake"
del /q "*.vcxproj"
del /q "*.sln"
del /q "*.filters"
ECHO [VTX] DONE
PAUSE