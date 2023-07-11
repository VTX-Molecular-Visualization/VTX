#include "app/component/chemistry/flat/bond.hpp"
#include "app/component/chemistry/flat/molecule.hpp"

namespace VTX::App::Component::Chemistry::Flat
{
	size_t Bond::getIndexFirstAtom() const { return _moleculePtr->_moleculeStruct.bondPairAtomIndexes[ _index * 2 ]; }
	void   Bond::setIndexFirstAtom( const size_t p_atomIndex )
	{
		_moleculePtr->_moleculeStruct.bondPairAtomIndexes[ _index * 2 ] = p_atomIndex;
	}
	size_t Bond::getIndexSecondAtom() const
	{
		return _moleculePtr->_moleculeStruct.bondPairAtomIndexes[ _index * 2 + 1 ];
	}
	void Bond::setIndexSecondAtom( const size_t p_atomIndex )
	{
		_moleculePtr->_moleculeStruct.bondPairAtomIndexes[ _index * 2 + 1 ] = p_atomIndex;
	}
} // namespace VTX::App::Component::Chemistry::Flat
