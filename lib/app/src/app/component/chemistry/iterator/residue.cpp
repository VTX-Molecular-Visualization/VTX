#include "app/component/chemistry/iterator/residue.hpp"
#include "app/component/chemistry/system.hpp"

namespace VTX::App::Component::Chemistry::Iterator
{
	ResidueContainer::ResidueIt::ResidueIt( System * const p_molecule, const size_t p_index, const size_t p_end ) :
		_molecule( p_molecule ), _index( p_index ), _end( p_end )
	{
		_getValid();
	}

	Residue & ResidueContainer::ResidueIt::operator*() const { return *( _molecule->getResidue( _index ) ); }
	Residue * ResidueContainer::ResidueIt::operator->() { return _molecule->getResidue( _index ); }

	// Prefix increment
	ResidueContainer::ResidueIt & ResidueContainer::ResidueIt::operator++()
	{
		_index++;
		_getValid();

		return *this;
	}

	// Postfix increment
	ResidueContainer::ResidueIt ResidueContainer::ResidueIt::operator++( int )
	{
		ResidueIt tmp = ResidueIt( _molecule, _index, _end );

		_index++;
		_getValid();

		return tmp;
	}

	bool operator==( const ResidueContainer::ResidueIt & a, const ResidueContainer::ResidueIt & b )
	{
		return a._molecule == b._molecule && a._index == b._index;
	}
	bool operator!=( const ResidueContainer::ResidueIt & a, const ResidueContainer::ResidueIt & b )
	{
		return a._molecule != b._molecule || a._index != b._index;
	}

	void ResidueContainer::ResidueIt::_getValid()
	{
		while ( _index != _end && _molecule->getResidue( _index ) == nullptr )
		{
			_index++;
		}
	}

	ResidueContainer::ResidueContainer(
		System * const p_molecule,
		const size_t   p_firstResidueIndex,
		const size_t   p_residueCount
	) : _molecule( p_molecule ), _firstResidueIndex( p_firstResidueIndex ), _residueCount( p_residueCount )
	{
	}

	ResidueContainer::ResidueIt ResidueContainer::begin()
	{
		return ResidueContainer::ResidueIt( _molecule, _firstResidueIndex, _firstResidueIndex + _residueCount );
	}
	ResidueContainer::ResidueIt ResidueContainer::end()
	{
		return ResidueContainer::ResidueIt(
			_molecule, _firstResidueIndex + _residueCount, _firstResidueIndex + _residueCount
		);
	}
} // namespace VTX::App::Component::Chemistry::Iterator
