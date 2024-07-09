# Change Log

## [0.4.4] (beta) - 2024-07-09

### Other

- Change download API to https://files.rcsb.org.

## [0.4.3] (beta) - 2024-04-29

### Debug

- Fix import trajectory pop-up.
- Fix trajectory loading with inconsistent atom count.
- Fix frame selection dropdown.

### UI

- Improve SES warning pop-up.

### Other

- Update chemfiles lib.

## [0.4.2] (beta) - 2024-02-29

### Debug

- Fix possible crash when loading big trajectory.
- Fix Memory leak when reading trajectory in SES representation.
- Debug crazy camera movement after focusing render panel.

### UI

- Display warning pop-up when trying to compute huge SES.
- Add feedback when loading a molecule.
- Display full path to image file when taking instant snapshot.
- Rename color modes Atom-Chain, Atom-Molecule, Atom-Custom into CHNOPS Chain, CHNOPS Molecule and CHNOPS Custom.
- Fix some typo.

## [0.4.1] (beta) - 2023-05-30

### Debug

- Fix issue with snapshot on scaled screens.
- Fix transparent outline on snapshots.
- Debug display of Position section of atom Inspector.
- Debug issue with Disulfide bond guessing on .cif files.
- Debug right click on Overlay which was applied on render.

### Optimizations

- Optimize duplicate frame submenu.
- Optimize selection.

### UI

- Improve resize area of docked panels.

### Other

- Update links in information to the new VTX git repository.

## [0.4.0] (beta) - 2023-05-04

### New features

- Add SES representation for molecules.
- Add an orthographic camera.
- Add functionality to compute RMSD and Structural Alignments on molecules.
- Add a layer called Category in the molecule hierarchy to group chains that contain the same type of residues (Categories are Polymer, Carbohydrate, Ligand, Ion, Solvent and Water).
- Add ability to load a trajectory file on a loaded molecule.
- Add a functionality to duplicate a single frame of a dynamic in a new molecule.

### Debug

- Debug issues with extract and duplicate functions.
- Debug issues with molecule export.
- Debug issues with sequence window.
- Debug transform modification when multiple molecules were selected.

### Optimizations

- Computation of SS (and SES) done when needed instead of compute it at molecule loading.
- Optimization of Show/Hide functions.
- Optimization of sequence generation for molecules with numerous unknown residues.
- Add a menu to display a specific inspector in the molecule hierarchy (i.e. display the inspector of all chains instead of the inspector of the molecule when the molecule is fully selected)

### UI

- Add a quick access to render effect presets in the render view.
- Visualization tab in main menu splitted into Camera and Molecule tabs.
- Export Molecule button duplicated in the Home tab.
- Add a quick access to background color in the main menu.
- Move camera parameters from Render Effects settings to VTX settings.
- Update Settings window layout.
- Display representations of children in the representation inspector.
- Display more data in the information section of the chain inspector.
- Limits number of bonds displayed in the inspector to 100.

### Other

- Update chemfiles to version 0.10.3.
- Split translation speed factor into acceleration and deceleration speed factor for camera movements.

## [0.3.0] (beta) - 2022-05-23

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
