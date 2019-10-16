# 1. Get VCPKG from  Github
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
# 2. Install VCPKG
.\bootstrap-vcpkg.bat
# 3. Integrate VCPKG
.\vcpkg integrate install
# 4. Install project dependencies
./vcpkg install sdl2:x64-windows gl3w:x64-windows imgui:x64-windows
# 5. Make solution
Run cmake.bat
# 6. Open solution
Open VTX.sln then set project "VTX" as startup project
