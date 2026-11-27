# Python API documentation: https://vtx.drugdesign.fr/doc/python-api/

"""Show only the atoms of the chosen chemical elements and hide all the others."""

import collections

import vtx

PDB_ID = "4HHB"
VISIBLE_SYMBOLS = {
    vtx.ATOM_SYMBOL.N,
    vtx.ATOM_SYMBOL.O,
    vtx.ATOM_SYMBOL.S,
    vtx.ATOM_SYMBOL.FE,
}

try:
    system = vtx.getSystem(PDB_ID)
except ValueError:
    vtx.download(PDB_ID)
    system = vtx.getSystem(PDB_ID)

system.setRepresentation("balls and sticks")

atoms = system.getAtoms()
atoms.setVisible(True)

hidden = []
counts = collections.Counter()
examples = {}
for atom, symbol in zip(atoms, atoms.symbols):
    if symbol in VISIBLE_SYMBOLS:
        counts[symbol] += 1
        examples.setdefault(symbol, atom)
    else:
        hidden.append(atom.index)

if hidden:
    system.getAtoms(hidden).setVisible(False)

print(f"{len(atoms) - len(hidden)} visible atoms, {len(hidden)} hidden")
for symbol, count in sorted(counts.items()):
    print(f"  {examples[symbol].symbolName}: {count}")
