# VTX
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/25aeed2cf0e54f45b39496354738bfc4)](https://www.codacy.com?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=sguionni/VTX&amp;utm_campaign=Badge_Grade)

## USER GUIDE

-   Install Visual C++: [link](https://support.microsoft.com/fr-fr/help/2977003/the-latest-supported-visual-c-downloads)
-   Download last release (VTX.rar in assets): [link](https://github.com/sguionni/VTX/releases)
-   Extract archive and run bin/VTX.exe
-   Report bugs, features, ideas or anything else by creating a new issue: [link](https://github.com/sguionni/VTX/issues)
-   Issues labeled "Discussion" need to be discuted together, let's debate!

### CONTROLS

#### Freefly

-   ZSQD or arrows keys to move the camera
-   Hold left mouse button and move to look around
-   Hold right mouse button and move to rotate on the side
-   Right click on item name in scene to open context menu
-   Left click on item in the scene view to open details in inspector view

#### Trackball

- Hold left/right mouse button and move to rotate the scene
- Hold wheel button and move to translate the rotation center
- Use wheel to zoom
- QDAERF to rotate
- ZS to zoom

### SHORTCUTS

- F1: Toggle camera controls
- F2: Change representation
- F3: Change color mode
- F4: Change shading
- F5: Add camera viewpoint at current position

### EXPORTS

- Snapshots are saved in the /snapshots folder
- Videos are saved in the /videos folder

### BUGS AND CRASH REPORTS

- A log file is saved in the /logs folder, please attach this file with your ticket

### SCRIPTABLE ACTIONS

command < param | param1 param2 ... | ... >

-   snapshot
-   change_representation < BALL_AND_STICK | VAN_DER_WAALS | STICK | SAS >
-   change_shading < LAMBERT | BLINN_PHONG | TOON | FLAT_COLOR >
-   change_color_mode < ATOM | RESIDUE | CHAIN | PROTEIN >
-   change_auto_rotate_speed < x y z | xyz > (float values between 0.0 and 1.0)

## CHANGELOG

### 0.0.6

TODO

### 0.0.5

- Multi representation
- Trackball controls
- Adaptive SSAO
- Outline shader
- Background color
- Options to change bond and atom radius

## DEVELOPPER GUIDE

### WINDOWS VISUAL STUDIO 2019 x64

#### 1. Make solution

-   Install Visual Studio 2019 latest version with C++ tools.  
-   Run _cmake.bat

#### 2. Open solution

Open "_windows/VTX.sln".

#### 3. Install/configure ClangFormat

Install [ClangFormat VS extension](https://marketplace.visualstudio.com/items?itemName=LLVMExtensions.ClangFormat).  
Go to Tools -> Options, then under LLVM/Clang, set the following values:
-   Format On Save
    -   Enable: True

-   Format Options
    -   Fallback Style: LLVM
    -   Sort includes: True
    -   Style: file

### Some architecture points

-   Action is designed to be undoable and launched by scripts. All menu items have to throw an action. An action can change the application state or stack workers.
-   State is an application state with its enabled/disabled UI components and its controllers (mouse, keyboard...). For example the visualization state handles the FPS camera controler, refresh the renderer and update the scene each frame.
-   Workers are functionalities that can be threaded, for example taking a snapshot or parse a file to load.

For example to load a file from the API:
Action OpenAPI launch the APIFetcher worker that fetch the server, then launch OpenAction with loaded data. This action run the Loader worker that parse data to create model and add result to the scene.
