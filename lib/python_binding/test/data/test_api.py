import PyTX.API

scene = PyTX.API.getScene()

molecule = scene.getMolecule("1AGA")
print("Molecule name : " + molecule.getName())

atom1 = molecule.getAtom(0)
print("First atom name : " + atom1.getName())

carbonCounter = 0

for atom in molecule.getAtoms():
    symbol = atom.getSymbol()
    if symbol == PyTX.API.ATOM_SYMBOL.A_C :
        carbonCounter += 1

print("{0} contains {1} carbons.".format(molecule.getName(), carbonCounter ) )

