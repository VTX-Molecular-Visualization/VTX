#include "app/component/chemistry/atom.hpp"
#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/component/chemistry/residue.hpp"
#include <core/chemdb/atom.hpp>

namespace VTX::App::Component::Chemistry
{
	Atom::Atom() : Core::Struct::Atom() {}
	Atom::Atom( Residue * const p_residue ) : Core::Struct::Atom( p_residue ) {};
	Atom::Atom( Molecule * const p_molecule ) : Core::Struct::Atom( p_molecule ) {};
	Atom::Atom( Molecule * const p_molecule, const size_t p_index ) : Core::Struct::Atom( p_molecule, p_index ) {};

	const Molecule * const Atom::getMoleculePtr() const { return static_cast<const Molecule *>( _moleculePtr ); }
	Molecule * const	   Atom::getMoleculePtr() { return static_cast<Molecule *>( _moleculePtr ); }

	void Atom::updateData()
	{
		getMoleculePtr()->setAtomRadius( _internalIndex, VTX::Core::ChemDB::Atom::SYMBOL_VDW_RADIUS[ int( _symbol ) ] );
		getMoleculePtr()->setAtomColor( _internalIndex, ChemDB::Atom::SYMBOL_COLOR[ int( _symbol ) ] );
	}

} // namespace VTX::App::Component::Chemistry
