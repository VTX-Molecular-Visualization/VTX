#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/residue.hpp"
#include "app/component/chemistry/system.hpp"

namespace VTX::App::Component::Chemistry
{
	Chain::Chain() {}

	Chain::Chain( System * const p_systemPtr, const size_t p_index ) :
		_systemPtr( p_systemPtr ), _index( p_index ) {};

	const std::string & Chain::getName() const { return _systemPtr->_systemStruct.chainNames[ _index ]; }
	void Chain::setName( const std::string & p_name ) { _systemPtr->_systemStruct.chainNames[ _index ] = p_name; }

	size_t Chain::getIndexFirstResidue() const { return _systemPtr->_systemStruct.chainFirstResidues[ _index ]; }
	size_t Chain::getIndexLastResidue() const
	{
		return _systemPtr->_systemStruct.chainFirstResidues[ _index ] + getResidueCount() - 1;
	}

	void Chain::setIndexFirstResidue( const size_t p_residueIndex )
	{
		_systemPtr->_systemStruct.chainFirstResidues[ _index ] = p_residueIndex;
	}

	size_t Chain::getResidueCount() const { return _systemPtr->_systemStruct.chainResidueCounts[ _index ]; }
	void   Chain::setResidueCount( const size_t p_residueCount )
	{
		_systemPtr->_systemStruct.chainResidueCounts[ _index ] = p_residueCount;
	}

	atom_index_t Chain::getIndexFirstAtom() const
	{
		const Residue * const firstResidue = _systemPtr->getResidue( getIndexFirstResidue() );
		return firstResidue->getIndexFirstAtom();
	}
	atom_index_t Chain::getIndexLastAtom() const
	{
		const Residue * const lastResidue = _systemPtr->getResidue( getIndexLastResidue() );
		return lastResidue->getIndexLastAtom();
	}

	ResidueIndexRange Chain::getResidueRange() const
	{
		return ResidueIndexRange( getIndexFirstResidue(), getResidueCount() );
	}
	AtomIndexRange Chain::getAtomRange() const
	{
		return AtomIndexRange::createFirstLast( getIndexFirstAtom(), getIndexLastAtom() );
	}

	Iterator::ResidueContainer Chain::residues() const
	{
		return Iterator::ResidueContainer( _systemPtr, getIndexFirstResidue(), getResidueCount() );
	}

	Iterator::AtomContainer Chain::atoms() const
	{
		const atom_index_t atomCount = getIndexLastAtom() - getIndexFirstAtom() + 1;
		return Iterator::AtomContainer( _systemPtr, getIndexFirstAtom(), atomCount );
	}

	bool Chain::isVisible() const
	{
		const AtomIndexRange atomRange = AtomIndexRange::createFirstLast( getIndexFirstAtom(), getIndexLastAtom() );
		return _systemPtr->_visibleAtomIds.intersectWith( atomRange );
	}
	bool Chain::isFullyVisible() const
	{
		const AtomIndexRange atomRange = AtomIndexRange::createFirstLast( getIndexFirstAtom(), getIndexLastAtom() );
		return _systemPtr->_visibleAtomIds.contains( atomRange );
	}

	void Chain::setVisible( const bool p_visible )
	{
		const AtomIndexRange atomRange = AtomIndexRange::createFirstLast( getIndexFirstAtom(), getIndexLastAtom() );
		_systemPtr->setVisible( atomRange, p_visible );
	}

	void Chain::remove()
	{
		const AtomIndexRange atomRange = AtomIndexRange::createFirstLast( getIndexFirstAtom(), getIndexLastAtom() );
		_systemPtr->remove( atomRange );
	}

} // namespace VTX::App::Component::Chemistry
