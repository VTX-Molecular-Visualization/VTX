#include "app/component/chemistry/bond.hpp"
#include "app/component/chemistry/molecule.hpp"

namespace VTX::App::Component::Chemistry
{
	size_t Bond::getIndexFirstAtom() const { return _moleculePtr->_bondAtomPairIds[ _internalIndex * 2 ]; }
	void   Bond::setIndexFirstAtom( const size_t p_atomIndex )
	{
		_moleculePtr->_bondAtomPairIds[ _internalIndex * 2 ] = p_atomIndex;
	}
	size_t Bond::getIndexSecondAtom() const { return _moleculePtr->_bondAtomPairIds[ _internalIndex * 2 + 1 ]; }
	void   Bond::setIndexSecondAtom( const size_t p_atomIndex )
	{
		_moleculePtr->_bondAtomPairIds[ _internalIndex * 2 + 1 ] = p_atomIndex;
	}
} // namespace VTX::App::Component::Chemistry
