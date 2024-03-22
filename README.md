# VTX - High performance molecular visualization software

## USER GUIDE

### Installation

#### Windows

- Install [Visual C++](https://support.microsoft.com/fr-fr/help/2977003/the-latest-supported-visual-c-downloads)
- Download [last release](https://github.com/VTX-Molecular-Visualization/VTX/releases)
- Run the installer (it will automatically extract files in the current folder)
- Launch VTX shortcut, or run in command line

#### Linux

Requirements :

* Ubuntu 20.04

* GLIBC 2.29

* GLIBCXX 3.4.28

Needed packages :

* From Ubuntu 22.04 :

build-essential libgl1-mesa-dev libopengl0 libfreetype6-dev libfontconfig1-dev libxkbcommon-dev libxkbcommon-x11-dev libxcb-icccm4-dev libxcb-image0-dev libxcb-keysyms1-dev libxcb-render-util0-dev libxcb-shape0-dev libxcb-randr0-dev libdbus-1-3 libsm-dev libpcre2-dev libjpeg-dev libtiff-dev libqt6openglwidgets6 libqt6network6 libqt6svg6 qt6-base-dev

* For Ubuntu 20.04 :

build-essential libgl1-mesa-dev libopengl0 libfreetype6-dev libfontconfig1-dev libxkbcommon-dev libxkbcommon-x11-dev libxcb-icccm4-dev libxcb-image0-dev libxcb-keysyms1-dev libxcb-render-util0-dev libxcb-shape0-dev libxcb-randr0-dev libdbus-1-3 libsm-dev libpcre2-dev libjpeg-dev libtiff-dev

Step by step :

- Install all the needed packages

- Download [last release](https://github.com/VTX-Molecular-Visualization/VTX/releases)

- Run bin/VTX

### Run in command line

Examples:
- ./VTX
- ./VTX 4hhb 4v6x
- ./VTX C:/save.vtx
- ./VTX C:/mol1.pdb C:/mol2.mmtf C:/mol3.mmcif

### Supported formats

Please [take a look there](https://chemfiles.org/chemfiles/latest/formats.html).

### Controllers

#### Trackball

- Hold left/right mouse button and move to rotate the scene
- Hold wheel button and move to translate the rotation center
- Use wheel to zoom
- ADQERF to rotate
- WS to zoom
- Hold Shift to speed up translation

#### Freefly

- ZASD or arrows keys to move the camera
- Hold left mouse button and move to look around
- Hold right mouse button and move to rotate on the side
- Hold Shift to speed up translation

### Shortcuts

- Ctrl + N : New session
- Ctrl + S : Save
- Ctrl + Shift + S : Save as...
- Ctrl + O : Open session/Molecule
- Ctrl + Shift + O : Download Molecule...

- F6: Restore the default layout
- F11: Toggle fullscreen

- Ctrl + A : Select all
- Escape: Clear the selection

- Ctrl + D : Duplicate selection
- Ctrl + E : Export selection
- Delete : Delete the selection
- O : Orient

- F1: Toggle camera controller
- CTRL + F1: Reset current camera controller
- F2: Switch current projection
- F3: Save current viewpoint
- F4: Toggle overlays visibility
- F5: Take a snapshot

- P : Switch to selection Picker mode
- M : Switch to Measure mode


### Snapshots

- Snapshots are saved in the /snapshots folder

### Render presets

- Render presets are saved in /libraries/render_effects folder
- You can manually add/remove files (will be updated at the next launch), this allows you to share presets between users

### Bugs and crash reports

- Report bugs, features, ideas or anything else by [creating a new issue](https://gitlab.com/VTX_mol/VTX/-/issues)
- A log file is saved in the /logs folder, please attach this file with your ticket

## BUILD FROM SOURCE

### Prerequisites

- C++20 compiler
- Git

### Windows

- Install Visual Studio, or Visual Code, or just MSVC compiler if you don't want to use any IDE
- Install [Conan 2](https://conan.io/downloads)
- Install [Python 3.9](https://www.python.org/downloads/)

### Create Conan profile

```
conan profile detect
```

Into `~/.conan2/profiles/default` add
```
[settings]

...

build_type=<Debug|Release>
```

### Add custom Conan repository

```
conan remote add --insecure cano7782 https://cano7782.odns.fr
```

### Set packages in editable mode

```
conan editable add lib/app
conan editable add lib/core
conan editable add lib/io
conan editable add lib/io/chemfiles
conan editable add lib/python_binding
conan editable add lib/renderer
conan editable add lib/tool
conan editable add lib/ui
conan editable add lib/util
conan editable add lib/tool/tools/mdprep
```

### Build VTX

```
conan build . -b missing -b editable -s compiler.cppstd=20 -s build_type=<Debug|Release>
```

**Note** : The first time conan compile external libraries, an assert may fail once and raise an error window that suspend the conan command. It doesn't mean the conan build failed, you can safely hit the `Retry` button to resume the conan build. 


## License

Please look at the license.txt file
