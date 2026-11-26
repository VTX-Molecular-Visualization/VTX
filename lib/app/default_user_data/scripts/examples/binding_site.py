# Python API documentation: https://vtx.drugdesign.fr/doc/python-api/

"""Show the binding site of a ligand: every residue with at least one atom close to it."""

import vtx

PDB_ID = "1HSG"
LIGAND_NAME = "MK1"
DISTANCE = 5.0

try:
    system = vtx.getSystem(PDB_ID)
except ValueError:
    vtx.download(PDB_ID)
    system = vtx.getSystem(PDB_ID)

system.setVisible(True)

ligand_indices = [residue.index for residue in system.getResidues() if residue.name == LIGAND_NAME]
if not ligand_indices:
    raise RuntimeError(f"No residue named {LIGAND_NAME} in {PDB_ID}")
ligand = system.getResidues(ligand_indices)

vtx.clearSelection()
ligand.setSelected()
vtx.selection.extend(DISTANCE)
vtx.selection.extendToResidue()
pocket = vtx.selection.getResidues(system)

system.setRepresentation("ribbons")
vtx.selection.setRepresentation("sticks")
ligand.setRepresentation("balls and sticks")
vtx.camera.orient(pocket, 500)

print(f"Residues within {DISTANCE} A of {LIGAND_NAME}:")
for residue in pocket:
    if residue.index not in ligand_indices:
        print(f"  {residue.name} {residue.originalId} (chain {residue.getChain().name})")
