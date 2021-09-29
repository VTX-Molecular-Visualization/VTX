# VTX - High performance molecular visualization software
[![Gitter](https://badges.gitter.im/VTX_mol/VTX.svg)](https://gitter.im/VTX_mol/VTX?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge)

## USER GUIDE

### Installation

#### Windows

- Install [Visual C++](https://support.microsoft.com/fr-fr/help/2977003/the-latest-supported-visual-c-downloads)
- Download [last release](https://gitlab.com/VTX_mol/VTX/-/releases)
- Run the installer (it will automatically extract files in the current folder)
- Launch VTX shortcut

#### Linux

- Install openssl and xinemara packages
- Download [last release](https://gitlab.com/VTX_mol/VTX/-/releases)
- Run bin/VTX

### CONTROLLERS

#### Trackball

- Hold left/right mouse button and move to rotate the scene
- Hold wheel button and move to translate the rotation center
- Use wheel to zoom
- ADQERF to rotate
- WS to zoom

#### Freefly

- ZASD or arrows keys to move the camera
- Hold left mouse button and move to look around
- Hold right mouse button and move to rotate on the side

### SHORTCUTS

- F1: Toggle camera controller
- CTRL + F1: Reset current camera controller
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

### SNAPSHOTS

- Snapshots are saved in the /snapshots folder

### RENDER PRESETS

- Render presets are saved in /libraries/render_effects folder
- You can manually add/remove files (will be updated at the next launch), this allows you to share presets between users

### BUGS AND CRASH REPORTS

- Report bugs, features, ideas or anything else by [creating a new issue](https://gitlab.com/VTX_mol/VTX/-/issues)
- A log file is saved in the /logs folder, please attach this file with your ticket

## LICENSE

Please look at the license.txt file