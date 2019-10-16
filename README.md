#1 Get VCPKG from  Github
git clone https://github.com/microsoft/vcpkg.git
#2 Enter repository
cd vcpkg
#3 Deploy
.\bootstrap-vcpkg.bat
#4 Integrate
.\vcpkg integrate install
#5 Install dependencies
./vcpkg install sdl2:x64-windows gl3w:x64-windows imgui:x64-windows
#6 Build
Run cmake.bat
#7 Open solution
Open VTX.sln then set project "VTX" as startup project