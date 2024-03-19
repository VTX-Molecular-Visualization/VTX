#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/component/chemistry/residue.hpp"

namespace VTX::App::Component::Chemistry
{
	Chain::Chain() {}

	Chain::Chain( Molecule * const p_moleculePtr, const size_t p_index ) :
		_moleculePtr( p_moleculePtr ), _index( p_index ) {};

	const std::string & Chain::getName() const { return _moleculePtr->_moleculeStruct.chainNames[ _index ]; }
	void Chain::setName( const std::string & p_name ) { _moleculePtr->_moleculeStruct.chainNames[ _index ] = p_name; }

	size_t Chain::getIndexFirstResidue() const { return _moleculePtr->_moleculeStruct.chainFirstResidues[ _index ]; }
	size_t Chain::getIndexLastResidue() const
	{
		return _moleculePtr->_moleculeStruct.chainFirstResidues[ _index ] + getResidueCount() - 1;
	}

	void Chain::setIndexFirstResidue( const size_t p_residueIndex )
	{
		_moleculePtr->_moleculeStruct.chainFirstResidues[ _index ] = p_residueIndex;
	}

	size_t Chain::getResidueCount() const { return _moleculePtr->_moleculeStruct.chainResidueCounts[ _index ]; }
	void   Chain::setResidueCount( const size_t p_residueCount )
	{
		_moleculePtr->_moleculeStruct.chainResidueCounts[ _index ] = p_residueCount;
	}

	atom_index_t Chain::getIndexFirstAtom() const
	{
		const Residue * const firstResidue = _moleculePtr->getResidue( getIndexFirstResidue() );
		return firstResidue->getIndexFirstAtom();
	}
	atom_index_t Chain::getIndexLastAtom() const
	{
		const Residue * const lastResidue = _moleculePtr->getResidue( getIndexLastResidue() );
		return lastResidue->getIndexLastAtom();
	}

	Iterator::ResidueContainer Chain::residues() const
	{
		return Iterator::ResidueContainer( _moleculePtr, getIndexFirstResidue(), getResidueCount() );
	}

	Iterator::AtomContainer Chain::atoms() const
	{
		const atom_index_t atomCount = getIndexLastAtom() - getIndexFirstAtom() + 1;
		return Iterator::AtomContainer( _moleculePtr, getIndexFirstAtom(), atomCount );
	}

	bool Chain::isVisible() const
	{
		const AtomIndexRange atomRange = AtomIndexRange::createFirstLast( getIndexFirstAtom(), getIndexLastAtom() );
		return _moleculePtr->_visibleAtomIds.intersectWith( atomRange );
	}
	bool Chain::isFullyVisible() const
	{
		const AtomIndexRange atomRange = AtomIndexRange::createFirstLast( getIndexFirstAtom(), getIndexLastAtom() );
		return _moleculePtr->_visibleAtomIds.contains( atomRange );
	}

	void Chain::setVisible( const bool p_visible )
	{
		const AtomIndexRange atomRange = AtomIndexRange::createFirstLast( getIndexFirstAtom(), getIndexLastAtom() );
		_moleculePtr->setVisible( atomRange, p_visible );
	}

	void Chain::remove()
	{
		const AtomIndexRange atomRange = AtomIndexRange::createFirstLast( getIndexFirstAtom(), getIndexLastAtom() );
		_moleculePtr->remove( atomRange );
	}

} // namespace VTX::App::Component::Chemistry
