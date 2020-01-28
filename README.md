# VTX 

[![Build Status](https://travis-ci.com/sguionni/VTX.svg?token=vS9x6LR1bWfHpTq6m8BU&branch=master)](https://travis-ci.com/sguionni/VTX)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/25aeed2cf0e54f45b39496354738bfc4)](https://www.codacy.com?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=sguionni/VTX&amp;utm_campaign=Badge_Grade)

## WINDOWS VISUAL STUDIO 2019 x64

### 1. Get VCPKG from  Github
    git clone https://github.com/microsoft/vcpkg.git
    cd vcpkg
### 2. Install VCPKG
    .\bootstrap-vcpkg.bat
### 3. Integrate VCPKG
    .\vcpkg integrate install
### 4. Install project dependencies
    .\vcpkg install sdl2:x64-windows gl3w:x64-windows glm:x64-windows msgpack:x64-windows assimp:x64-windows
### 5. Make solution
Install Visual Studio 2019 latest version with C++ tools.  
Run cmake_build_VS2019_x64.bat
### 6. Open solution
Open "_windows/VTX.sln" then set project "VTX" as startup project.
### 7. Install/configure ClangFormat
Install [ClangFormat VS extension](https://marketplace.visualstudio.com/items?itemName=LLVMExtensions.ClangFormat).  
Go to Tools -> Options, then under LLVM/Clang, set the following values:
-   Format On Save
    -   Enable: True

-   Format Options
    -   Fallback Style: LLVM
    -   Sort includes: True
    -   Style: file

## LINUX

Pareil mais penser à installer git, curl, cmake et GL (libgl1-mesa-dev) avant vcpkg.
## MAC OS