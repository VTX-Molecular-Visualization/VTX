@ECHO OFF
ECHO [VTX] CLONING VCPKG
git clone https://github.com/microsoft/vcpkg.git
.\vcpkg\bootstrap-vcpkg.bat
ECHO [VTX] INSTALLING VCPKG
.\vcpkg\vcpkg integrate install
.\vcpkg\vcpkg install sdl2:x64-windows gl3w:x64-windows glm:x64-windows msgpack:x64-windows
ECHO [VTX] DONE
PAUSE