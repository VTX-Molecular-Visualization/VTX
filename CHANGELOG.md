# Change Log

## 2026.11.0

### Python scripting

- Added Python API ([documentation](https://vtx.drugdesign.fr/doc/python-api/))
- Added example scripts
- Added a built-in Python script editor
- Added a user scripts folder

### Features

- Added ribbon color schemes
- Added selection extension:
  - Select all atoms within a given distance (Å) of the current selection
  - Extend the selection to complete residues
  - Invert the current selection

### UI

- Sequence viewer now uses author residue IDs by default (can be changed in settings)
- Added a new status screen to follow preparation steps in MdPrep tool
- Added image export dialog (resolution, background opacity, metadata)
- Added trajectory loading progress
- Added environment map selector with thumbnails
- Prevented residue labels from overlapping in the sequence viewer

### Performance

- Reduced memory usage when loading large trajectories
- Improved multi-threading
- Improved error handling
- Reduced CPU usage when idle

### Accessibility

- Added a colorblind-friendly color mode in settings
- Added an option to use the OpenDyslexic font

### Linux

- Now uses the dedicated NVIDIA GPU by default on hybrid laptops
- Added native Wayland support

### Fixes

- Fixed colors of exported images (now correctly encoded in sRGB)
- Added an explicit error when the export resolution exceeds the GPU limit
- Fixed trajectory playback skipping the last frame
- Fixed MdPrep default force field differing between operating systems

## 2026.6.2

- Added tool: molecular dynamics preparation with GROMACS
- Added camera viewpoint saving
- Added environment map support (.hdr, .exr)
- Added gamma-correct rendering
- Improved bond recomputation
- Improved various UI components

## 2026.6.1

- Added more SES settings
- Updated the auto-updater
- Added the ability to switch the tree view between chains and categories
- Added new carbon-based color schemes

## 2026.6.0

- New VTX beta
