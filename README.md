# VTX
## WINDOWS VISUAL STUDIO 2019 x64

### 1. Get VCPKG from  Github
    git clone https://github.com/microsoft/vcpkg.git
    cd vcpkg
### 2. Install VCPKG
    .\bootstrap-vcpkg.bat
### 3. Integrate VCPKG
    .\vcpkg integrate install
### 4. Install project dependencies
    .\vcpkg install sdl2:x64-windows gl3w:x64-windows imgui:x64-windows glm:x64-windows msgpack:x64-windows
### 5. Make solution
Run cmake_VS2019_x64.bat
### 6. Open solution
Open "_windows/VTX.sln" then set project "VTX" as startup project
### 7. Install/configure ClangFormat
Install [ClangFormat VS extension](https://marketplace.visualstudio.com/items?itemName=LLVMExtensions.ClangFormat)
Go to Tools -> Options, then under LLVM/Clang, set the followiung values:
- Format On Save
	- Enable: True
- Format Options
	- Fallback Style: LLVM
	- Sort includes: True
	- Style: file

## LINUX

## MAC OS


