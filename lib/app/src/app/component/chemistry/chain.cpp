#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/residue.hpp"
#include "app/component/chemistry/system.hpp"

namespace VTX::App::Component::Chemistry
{
	Chain::Chain() {}

	Chain::Chain( System * const p_systemPtr, const Index p_index ) : _systemPtr( p_systemPtr ), _index( p_index ) {};

	const std::string & Chain::getName() const { return _systemPtr->_systemStruct.chainNames[ _index ]; }

	void Chain::setName( const std::string & p_name ) { _systemPtr->_systemStruct.chainNames[ _index ] = p_name; }

	Index Chain::getIndexFirstResidue() const { return _systemPtr->_systemStruct.chainFirstResidues[ _index ]; }

	Index Chain::getIndexLastResidue() const
	{
		return _systemPtr->_systemStruct.chainFirstResidues[ _index ] + getResidueCount() - 1;
	}

	void Chain::setIndexFirstResidue( const Index p_residueIndex )
	{
		_systemPtr->_systemStruct.chainFirstResidues[ _index ] = p_residueIndex;
	}

	Index Chain::getResidueCount() const { return _systemPtr->_systemStruct.chainResidueCounts[ _index ]; }

	void Chain::setResidueCount( const Index p_residueCount )
	{
		_systemPtr->_systemStruct.chainResidueCounts[ _index ] = p_residueCount;
	}

	Index Chain::getIndexFirstAtom() const
	{
		const Residue * const firstResidue = _systemPtr->getResidue( getIndexFirstResidue() );
		return firstResidue->getIndexFirstAtom();
	}

	Index Chain::getIndexLastAtom() const
	{
		const Residue * const lastResidue = _systemPtr->getResidue( getIndexLastResidue() );
		return lastResidue->getIndexLastAtom();
	}

	IndexRange Chain::getResidueRange() const { return IndexRange( getIndexFirstResidue(), getResidueCount() ); }

	IndexRange Chain::getAtomRange() const
	{
		return IndexRange::createFirstLast( getIndexFirstAtom(), getIndexLastAtom() );
	}

	Iterator::ResidueContainer Chain::residues() const
	{
		return Iterator::ResidueContainer( _systemPtr, getIndexFirstResidue(), getResidueCount() );
	}

	Iterator::AtomContainer Chain::atoms() const
	{
		const Index atomCount = getIndexLastAtom() - getIndexFirstAtom() + 1;
		return Iterator::AtomContainer( _systemPtr, getIndexFirstAtom(), atomCount );
	}

	bool Chain::isVisible() const
	{
		const IndexRange atomRange = IndexRange::createFirstLast( getIndexFirstAtom(), getIndexLastAtom() );
		return _systemPtr->_visibleAtomIds.intersectWith( atomRange );
	}

	bool Chain::isFullyVisible() const
	{
		const IndexRange atomRange = IndexRange::createFirstLast( getIndexFirstAtom(), getIndexLastAtom() );
		return _systemPtr->_visibleAtomIds.contains( atomRange );
	}

	void Chain::setVisible( const bool p_visible )
	{
		const IndexRange atomRange = IndexRange::createFirstLast( getIndexFirstAtom(), getIndexLastAtom() );
		_systemPtr->setVisible( atomRange, p_visible );
	}

	void Chain::remove()
	{
		const IndexRange atomRange = IndexRange::createFirstLast( getIndexFirstAtom(), getIndexLastAtom() );
		_systemPtr->remove( atomRange );
	}

} // namespace VTX::App::Component::Chemistry
