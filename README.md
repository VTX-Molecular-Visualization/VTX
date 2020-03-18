# VTX 

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/25aeed2cf0e54f45b39496354738bfc4)](https://www.codacy.com?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=sguionni/VTX&amp;utm_campaign=Badge_Grade)

## WINDOWS VISUAL STUDIO 2019 x64

### 1. Make solution
Install Visual Studio 2019 latest version with C++ tools.  
Run _cmake.bat
### 2. Open solution
Open "_windows/VTX.sln".
### 3. Install/configure ClangFormat
Install [ClangFormat VS extension](https://marketplace.visualstudio.com/items?itemName=LLVMExtensions.ClangFormat).  
Go to Tools -> Options, then under LLVM/Clang, set the following values:
-   Format On Save
    -   Enable: True

-   Format Options
    -   Fallback Style: LLVM
    -   Sort includes: True
    -   Style: file

## LINUX

git curl cmake GL (libgl1-mesa-dev)

## MAC OS