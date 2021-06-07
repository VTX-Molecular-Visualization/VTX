@ECHO OFF
ECHO [VTX] CLEANING SOLUTION
rmdir /s /q "../_windows/CMakeFiles"
rmdir /s /q "../_windows/.vs"
rmdir /s /q "../_windows/VTX.dir"
rmdir /s /q "../_windows/x64"
rmdir /s /q "../_windows/Debug"
rmdir /s /q "../_windows/Release"
rmdir /s /q "../_windows/RelWithDebInfo"
rmdir /s /q "../_windows/build"
rmdir /s /q "../_windows/VTX_autogen"
del /q "../_windows/CMakeCache.txt"
del /q "../_windows/*.cmake"
del /q "../_windows/*.vcxproj"
del /q "../_windows/*.sln"
del /q "../_windows/*.filters"
del /q "../_windows/*.user"
ECHO [VTX] DONE
PAUSE