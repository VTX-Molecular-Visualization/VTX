cmake .. -B "../_windows" -G "Visual Studio 16 2019" -DCMAKE_GENERATOR_PLATFORM=x64 -DDEFINE_PRODUCTION=ON .
cmake --build ../_windows --config Release --clean-first
robocopy ../_windows/build build /nfl /ndl /njh /E
rmdir /s /q build\bin\Debug
rmdir /s /q build\bin\RelWithDebInfo
rmdir /s /q build\libraries\representations
rmdir /s /q build\snapshots
rmdir /s /q build\logs
del /s /q build\config.ini
del /s /q build\setting.json
del /s /q build\libraries\render_effects\Default
cmake .. -B "../_windows" -G "Visual Studio 16 2019" -DCMAKE_GENERATOR_PLATFORM=x64 -DDEFINE_PRODUCTION=OFF .
PAUSE