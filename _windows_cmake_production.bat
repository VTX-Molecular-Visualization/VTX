@ECHO OFF
cmake -B build -DDEFINE_PRODUCTION=ON .
cmake --build build --config Release --clean-first
robocopy build/build RELEASE /nfl /ndl /njh /E
rmdir /s /q RELEASE/bin/Debug
rmdir /s /q RELEASE/bin/RelWithDebInfo
rmdir /s /q RELEASE/libraries/representations
rmdir /s /q RELEASE/snapshots
rmdir /s /q RELEASE/logs
del /s /q RELEASE/config.ini
del /s /q RELEASE/setting.json
del /s /q RELEASE/libraries/render_effects/Default
PAUSE