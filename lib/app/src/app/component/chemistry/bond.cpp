#include "app/component/chemistry/bond.hpp"
#include "app/component/chemistry/molecule.hpp"

namespace VTX::App::Component::Chemistry
{
	Bond::Bond( Molecule * const p_moleculePtr ) : Core::Struct::Bond( p_moleculePtr ) {}
	Bond::Bond( Molecule * const p_moleculePtr, const size_t p_index ) : Core::Struct::Bond( p_moleculePtr, p_index ) {}

	const Molecule * const Bond::getMoleculePtr() const { return static_cast<const Molecule *>( _moleculePtr ); }
	Molecule * const	   Bond::getMoleculePtr() { return static_cast<Molecule *>( _moleculePtr ); }

	size_t Bond::getIndexFirstAtom() const { return getMoleculePtr()->_bondAtomPairIds[ _internalIndex * 2 ]; }
	void   Bond::setIndexFirstAtom( const size_t p_atomIndex )
	{
		getMoleculePtr()->_bondAtomPairIds[ _internalIndex * 2 ] = uint( p_atomIndex );
	}
	size_t Bond::getIndexSecondAtom() const { return getMoleculePtr()->_bondAtomPairIds[ _internalIndex * 2 + 1 ]; }
	void   Bond::setIndexSecondAtom( const size_t p_atomIndex )
	{
		getMoleculePtr()->_bondAtomPairIds[ _internalIndex * 2 + 1 ] = uint( p_atomIndex );
	}
} // namespace VTX::App::Component::Chemistry
