# VTX - High performance molecular visualization software
[![Gitter](https://badges.gitter.im/VTX_mol/VTX.svg)](https://gitter.im/VTX_mol/VTX?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge)

![VTX Image](https://vtx.drugdesign.fr/assets/snapshots/2.png)

## USER GUIDE

### Installation

#### Windows

- Install [Visual C++](https://support.microsoft.com/fr-fr/help/2977003/the-latest-supported-visual-c-downloads)
- Download [last release](https://gitlab.com/VTX_mol/VTX/-/releases)
- Run the installer (it will automatically extract files in the current folder)
- Launch VTX shortcut, or run in command line

#### Linux

- Install openssl and xinerama packages
- Download [last release](https://gitlab.com/VTX_mol/VTX/-/releases)
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

- F1: Toggle camera controller
- CTRL + F1: Reset current camera controller
- F2: Add current viewpoint
- O : Orient
- F5: Take a snapshot
- F6: Restore the default layout
- F11: Toggle fullscreen
- Escape: Clear the selection
- Delete : Delete the selection
- Ctrl + S : Save
- Ctrl + Shift + S : Save as...
- Ctrl + N : New session
- Ctrl + O : Open session
- Ctrl + A : Select all
- Ctrl + D : Duplicate selection
- Ctrl + E : Export selection

### Snapshots

- Snapshots are saved in the /snapshots folder

### Render presets

- Render presets are saved in /libraries/render_effects folder
- You can manually add/remove files (will be updated at the next launch), this allows you to share presets between users

### Bugs and crash reports

- Report bugs, features, ideas or anything else by [creating a new issue](https://gitlab.com/VTX_mol/VTX/-/issues)
- A log file is saved in the /logs folder, please attach this file with your ticket

## BUILD FROM SOURCE (Available soon)

### Prerequisites

- C++17 compiler (MSVC or GCC)
- Git
- CMake
- Qt 5.15.1

### Windows

- Install Visual Studio 2019, or Visual Code, or just MSVC compiler if you don't want to use any IDE
- Install Qt 5.15.1 with [Qt Online Installer](https://www.qt.io/download-qt-installer) (only "MSVC 219 64-bit" is needed)
- Add CMake to your "Path" environment variable: \<dir\>/CMake/bin
- Create an environment variable "Qt5_DIR" with value: \<dir\>/Qt/5.15.1/msvc2019_64

#### Create Visual Studio solution with CMake

```
git clone https://gitlab.com/VTX_mol/VTX.git
cd VTX
cmake -B build .
```

It will build external libraries and create the solution in the build folder.
Others dependencies are downloaded during the first compilation.

#### Build in command line

If you don't want to use Visual Studio, you can also build executable from command line:

```
cd VTX
cmake --build build --config <Release|Debug>
```

### Linux

#### Create makefile with CMake

```
git clone https://gitlab.com/VTX_mol/VTX.git
cd VTX
cmake -B build --config <Release|Debug> .
```

#### Build a release in command line

```
cd VTX
cd build
make
```

### CMake options

- DEFINE_PRODUCTION: build for production usage (disable some dev features)
- BUILD_LIB: build libraries (you can disable this option once the /lib folder is generated

Example:

```
cmake -B build -DDEFINE_PRODUCTION=ON -DBUILD_LIB=OFF .
```

## License

Please look at the license.txt file