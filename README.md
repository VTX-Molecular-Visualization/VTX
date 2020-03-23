# VTX USER GUIDE

-   Install Visual C++: https://support.microsoft.com/fr-fr/help/2977003/the-latest-supported-visual-c-downloads
-   Run bin/VTX.exe

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/25aeed2cf0e54f45b39496354738bfc4)](https://www.codacy.com?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=sguionni/VTX&amp;utm_campaign=Badge_Grade)

# VTX DEV GUIDE

## WINDOWS VISUAL STUDIO 2019 x64

### 1. Make solution
-   Install Visual Studio 2019 latest version with C++ tools.  
-   Run _cmake.bat

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

## Some architecture points
- Action is designed to be undoable and launched by scripts. All menu items have to throw an action. An action can change the application state or stack workers.
- State is an application state with its enabled/disabled UI components and its controllers (mouse, keyboard...). For example the visualization state handles the FPS camera controler, refresh the renderer and update the scene each frame.
- Workers are functionalities that can be threaded, for example taking a snapshot or parse a file to load.

For example to load a file from the API:
Action OpenAPI launch the APIFetcher worker that fetch the server, then launch OpenAction with loaded data. This action run the Loader worker that parse data to create model and add result to the scene.