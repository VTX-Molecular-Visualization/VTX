# Python API documentation: https://vtx.drugdesign.fr/doc/python-api/

"""Highlight ligands and ions: hide water and show them as balls and sticks over the ribbons."""

import vtx

PDB_ID = "4HHB"

try:
    system = vtx.getSystem(PDB_ID)
except ValueError:
    vtx.download(PDB_ID)
    system = vtx.getSystem(PDB_ID)

system.setRepresentation("ribbons")
for category in system.getCategories():
    if category.residueCount == 0:
        continue
    if category.name == "Water":
        category.getResidues().setVisible(False)
    elif category.name in ("Ligand", "Ion"):
        category.getResidues().setRepresentation("balls and sticks")
        print(f"{category.name}: {', '.join(sorted({residue.name for residue in category.getResidues()}))}")
