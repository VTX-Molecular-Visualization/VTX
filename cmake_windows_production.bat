cmake -B build -G -DDEFINE_PRODUCTION=ON .
cmake --build ../_windows --config Release --clean-first
robocopy ../_windows/build RELEASE /nfl /ndl /njh /E
rmdir /s /q RELEASE\bin\Debug
rmdir /s /q RELEASE\bin\RelWithDebInfo
rmdir /s /q RELEASE\libraries\representations
rmdir /s /q RELEASE\snapshots
rmdir /s /q RELEASE\logs
del /s /q RELEASE\config.ini
del /s /q RELEASE\setting.json
del /s /q RELEASE\libraries\render_effects\Default
cmake -B build -DDEFINE_PRODUCTION=OFF .
PAUSE