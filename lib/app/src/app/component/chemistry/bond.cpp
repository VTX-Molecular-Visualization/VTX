#include "app/component/chemistry/bond.hpp"
#include "app/component/chemistry/molecule.hpp"

namespace VTX::App::Component::Chemistry
{
	atom_index_t Bond::getIndexFirstAtom() const
	{
		return _moleculePtr->_moleculeStruct.bondPairAtomIndexes[ _index * 2 ];
	}
	void Bond::setIndexFirstAtom( const atom_index_t p_atomIndex )
	{
		_moleculePtr->_moleculeStruct.bondPairAtomIndexes[ _index * 2 ] = p_atomIndex;
	}
	atom_index_t Bond::getIndexSecondAtom() const
	{
		return _moleculePtr->_moleculeStruct.bondPairAtomIndexes[ _index * 2 + 1 ];
	}
	void Bond::setIndexSecondAtom( const atom_index_t p_atomIndex )
	{
		_moleculePtr->_moleculeStruct.bondPairAtomIndexes[ _index * 2 + 1 ] = p_atomIndex;
	}
} // namespace VTX::App::Component::Chemistry
