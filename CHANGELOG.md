# Change Log

## [0.3.0] (beta) - 2021-03-18

### New features

- Measure tool.
- Freeze inspector button.
- Add color blending mode in representations.
- Add contextual menu on viewpoint container.
- Manage "Go to" function of viewpoints with multiple selection.

### Debug

- Debug preview opacity when exporting an image.
- Debug chain / residue / atom count update in molecule inspector after a modification in the molecule structure.
- Improve stability after an extract.
- Debug orient shortcut without selection.
- Applying a representation without selection apply it on every molecule instead of the first one only.
- Debug camera clip refresh after a reset.
- Debug picking on scaled screens.
- Debug negative scale using drag on transform widget.

### Other

- Upgrade interface library to Qt6.
- Rearrange visualization buttons in main menu.
- Improve widget to set fog clip and camera clip in render effects.


## [0.2.0] (beta) - 2021-12-13

- Picking (selection in 3D)
- Advanced image exporter
- Creation of Viewpoints to record a camera point of view
- Trajectory can be open directly on a molecule
- Can open files or download molecules in command line
- New visibility actions :
	- "Solo" action to hide everything else than the target
	- "Show All" action to show every objects in the scene
- Add atom name in atom's inspector
- Save and library files are now cross-plateform
- Export and Fullscreen buttons has been relocated in the main menu
- Library reloading debugged
- Selection optimized
- New UI toolbar overlay

## [0.1.2] (beta) - 2021-10-20

- Check for an available update at startup
- Read mmCIF assemblies
- Main menu improved. Molecule section has been removed, "Download" action has moved to "File" section and "Export" action to "Selection Action" section
- Portable save option added (the save may be longer but can be shared on different devices)
- MMCIF parser optimization

## [0.1.1] (beta) - 2021-09-29

The first release of VTX.
Feel free to give your feedback in the public [issue tracker](https://gitlab.com/VTX_mol/VTX/-/issues).
