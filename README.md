# VTX - High performance molecular visualization software

## USER GUIDE

- Install [Visual C++](https://support.microsoft.com/fr-fr/help/2977003/the-latest-supported-visual-c-downloads)
- Download [last release](https://gitlab.com/VTX_mol/VTX/-/releases)
- Run the installer (it will automatically extract files in the current folder)
- Launch VTX
- Report bugs, features, ideas or anything else by [creating a new issue](https://gitlab.com/VTX_mol/VTX/-/issues)

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
- You can manually add/remove files (will be updated at the next launch), this allows you to share presets between users.

### BUGS AND CRASH REPORTS

- A log file is saved in the /logs folder, please attach this file with your ticket

## LICENSE

Please look at the license.txt file

## Linux compilation notes

sudo apt-get install build-essential cmake libfontconfig1-dev libfreetype6-dev libx11-dev libx11-xcb-dev libxext-dev libxfixes-dev libxi-dev libxrender-dev libxcb1-dev libxcb-glx0-dev libxcb-keysyms1-dev libxcb-image0-dev libxcb-shm0-dev libxcb-icccm4-dev libxcb-sync0-dev libxcb-xfixes0-dev libxcb-shape0-dev libxcb-randr0-dev libxcb-render-util0-dev libxcb-xinerama0-dev libxkbcommon-dev libxkbcommon-x11-dev mesa-utils

//libpcre2-dev libssl-dev


cd Qt/version/Src

sudo ./configure -opensource -release -static -prefix /home/sguionni/Qt_static -nomake tools -nomake tests -nomake examples -bundled-xcb-xinput -no-icu -qt-pcre -openssl-linked

sudo tar -czvf build.tar.gz build

sudo code . --user-data-dir="/home/vscode" --no-sandbox