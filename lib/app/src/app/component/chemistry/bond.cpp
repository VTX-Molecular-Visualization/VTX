#include "app/component/chemistry/bond.hpp"
#include "app/component/chemistry/atom.hpp"
#include "app/component/chemistry/system.hpp"

namespace VTX::App::Component::Chemistry
{
	atom_index_t Bond::getIndexFirstAtom() const
	{
		return _systemPtr->_systemStruct.bondPairAtomIndexes[ _index * 2 ];
	}
	void Bond::setIndexFirstAtom( const atom_index_t p_atomIndex )
	{
		_systemPtr->_systemStruct.bondPairAtomIndexes[ _index * 2 ] = p_atomIndex;
	}
	atom_index_t Bond::getIndexSecondAtom() const
	{
		return _systemPtr->_systemStruct.bondPairAtomIndexes[ _index * 2 + 1 ];
	}
	void Bond::setIndexSecondAtom( const atom_index_t p_atomIndex )
	{
		_systemPtr->_systemStruct.bondPairAtomIndexes[ _index * 2 + 1 ] = p_atomIndex;
	}

	bool Bond::isVisible() const
	{
		const Chemistry::Atom * const atom1 = _systemPtr->getAtom( getIndexFirstAtom() );
		const Chemistry::Atom * const atom2 = _systemPtr->getAtom( getIndexSecondAtom() );

		return atom1 != nullptr && atom1->isVisible() && atom2 != nullptr && atom2->isVisible();
	}
	void Bond::setVisible( const bool p_visible )
	{
		AtomIndexRangeList atomRange = AtomIndexRangeList();

		atomRange.addValue( getIndexFirstAtom() );
		atomRange.addValue( getIndexSecondAtom() );

		_systemPtr->setVisible( atomRange, p_visible );
	}
} // namespace VTX::App::Component::Chemistry
