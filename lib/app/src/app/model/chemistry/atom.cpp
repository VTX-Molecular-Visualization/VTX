#include "app/model/chemistry/atom.hpp"
#include "app/model/chemistry/chain.hpp"
#include "app/model/chemistry/molecule.hpp"
#include "app/model/chemistry/residue.hpp"
#include <core/chemdb/atom.hpp>

namespace VTX::App::Model::Chemistry
{
	Atom::Atom() : AtomCore() {}
	Atom::Atom( Residue * const p_residue ) : AtomCore( p_residue ) {};
	Atom::Atom( Molecule * const p_molecule ) : AtomCore( p_molecule ) {};
	Atom::Atom( Molecule * const p_molecule, const size_t p_index ) : AtomCore( p_molecule, p_index ) {};

	void Atom::updateData()
	{
		_moleculePtr->setAtomRadius( _internalIndex, VTX::Core::ChemDB::Atom::SYMBOL_VDW_RADIUS[ int( _symbol ) ] );
		_moleculePtr->setAtomColor( _internalIndex, ChemDB::Atom::SYMBOL_COLOR[ int( _symbol ) ] );
	}

} // namespace VTX::App::Model::Chemistry
