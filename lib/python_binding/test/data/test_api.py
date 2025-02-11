import vtx_python_api.API
import vtx_python_api.Core

def countAtoms(p_selection):

    print(p_selection)
    
    mapAtomCounter = {}

    for atom in p_selection.getAtoms():
        symbol = atom.getSymbol()
        mapAtomCounter[symbol] = mapAtomCounter.get(symbol, 0) + 1

    if len(mapAtomCounter) == 0:
        print("0 atom founds")
    else:
        for atomSymbol, atomCount in mapAtomCounter.items() :
            print("target contains {0} {1}.".format(atomSymbol.name, atomCount ) )

vtx_python_api.Core.addCommand("countAtoms", countAtoms)
# scene = PyTX.API.getScene()

# molecule = scene.getMolecule("1AGA")
# print("Molecule name : " + molecule.getName())

# atom1 = molecule.getAtom(0)
# print("First atom name : " + atom1.getName())

# countAtoms(molecule)