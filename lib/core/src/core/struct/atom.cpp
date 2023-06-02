#include "core/struct/atom.hpp"
#include "core/struct/chain.hpp"
#include "core/struct/molecule.hpp"
#include "core/struct/residue.hpp"

namespace VTX::Core::Struct
{
	Molecule * const Atom::getMoleculePtr() const { return _residuePtr->getChainPtr()->getMoleculePtr(); };
	Chain * const	 Atom::getChainPtr() const { return _residuePtr->getChainPtr(); }

	// const Vec3f & Atom::getLocalPosition() const { return getLocalPosition( getMoleculePtr()->getFrame() ); }
	const Vec3f & Atom::getLocalPosition( const uint & p_frameIndex ) const
	{
		return getMoleculePtr()->getAtomPositionFrame( p_frameIndex )[ _index ];
	}
} // namespace VTX::Core::Struct
