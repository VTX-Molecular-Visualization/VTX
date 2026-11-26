"""Download a structure from the Protein Data Bank and print a summary of its content."""

import vtx

PDB_ID = "4HHB"

try:
    system = vtx.getSystem(PDB_ID)
except ValueError:
    vtx.download(PDB_ID)
    system = vtx.getSystem(PDB_ID)

print(f"{system.name} ({system.pdbIdCode})")
print(
    f"{system.chainCount} chains, {system.residueCount} residues, "
    f"{system.atomCount} atoms, {system.bondCount} bonds"
)

print("Categories:")
for category in system.getCategories():
    if category.residueCount > 0:
        print(f"  {category.name}: {category.residueCount} residues, {category.atomCount} atoms")

print("Chains:")
for chain in system.getChains():
    print(f"  {chain.name}: {chain.residueCount} residues, {chain.atomCount} atoms")
