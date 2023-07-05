#include "core/struct/atom.hpp"
#include "core/struct/chain.hpp"
#include "core/struct/molecule.hpp"
#include "core/struct/residue.hpp"

namespace VTX::Core::Struct
{
	Atom::Atom() : AtomCore() {}
	Atom::Atom( Residue * const p_residue ) : AtomCore( p_residue ) {};
	Atom::Atom( Molecule * const p_molecule ) : AtomCore( p_molecule ) {};
	Atom::Atom( Molecule * const p_molecule, const size_t p_internalIndex ) :
		AtomCore( p_molecule, p_internalIndex ) {};
	Atom::Atom( MoleculeCore * const p_molecule, const size_t p_internalIndex ) :
		AtomCore( static_cast<Molecule * const>( p_molecule ), p_internalIndex ) {};
} // namespace VTX::Core::Struct
