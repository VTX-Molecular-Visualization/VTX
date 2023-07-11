#include "app/component/chemistry/flat/chain.hpp"
#include "app/component/chemistry/flat/molecule.hpp"

namespace VTX::App::Component::Chemistry::Flat
{
	Chain::Chain() {}

	Chain::Chain( Molecule * const p_moleculePtr, const size_t p_index ) :
		_moleculePtr( p_moleculePtr ), _index( p_index ) {};

	const std::string & Chain::getName() const { return _moleculePtr->_moleculeStruct.chainNames[ _index ]; }
	void Chain::setName( const std::string & p_name ) { _moleculePtr->_moleculeStruct.chainNames[ _index ] = p_name; }

	const size_t Chain::getIndexFirstResidue() const
	{
		return _moleculePtr->_moleculeStruct.chainFirstResidues[ _index ];
	}
	void Chain::setIndexFirstResidue( const size_t p_residueIndex )
	{
		_moleculePtr->_moleculeStruct.chainFirstResidues[ _index ] = p_residueIndex;
	}

	const size_t Chain::getResidueCount() const { return _moleculePtr->_moleculeStruct.chainResidueCounts[ _index ]; }
	void		 Chain::setResidueCount( const size_t p_residueCount )
	{
		_moleculePtr->_moleculeStruct.chainResidueCounts[ _index ] = p_residueCount;
	}

} // namespace VTX::App::Component::Chemistry::Flat
