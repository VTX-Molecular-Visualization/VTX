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

	size_t Chain::getIndexFirstAtom() const
	{
		const Residue * const firstResidue = _moleculePtr->getResidue( getIndexFirstResidue() );
		return firstResidue->getIndexFirstAtom();
	}
	size_t Chain::getIndexLastAtom() const
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
		const size_t atomCount = getIndexLastAtom() - getIndexFirstAtom() + 1;
		return Iterator::AtomContainer( _moleculePtr, getIndexFirstAtom(), atomCount );
	}

} // namespace VTX::App::Component::Chemistry
