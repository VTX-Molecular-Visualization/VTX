#include "app/component/chemistry/iterator/residue.hpp"
#include "app/component/chemistry/system.hpp"

namespace VTX::App::Component::Chemistry::Iterator
{
	ResidueContainer::ResidueIt::ResidueIt( System * const p_system, const size_t p_index, const size_t p_end ) :
		_system( p_system ), _index( p_index ), _end( p_end )
	{
		_getValid();
	}

	Residue & ResidueContainer::ResidueIt::operator*() const { return *( _system->getResidue( _index ) ); }
	Residue * ResidueContainer::ResidueIt::operator->() { return _system->getResidue( _index ); }

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
		ResidueIt tmp = ResidueIt( _system, _index, _end );

		_index++;
		_getValid();

		return tmp;
	}

	bool operator==( const ResidueContainer::ResidueIt & a, const ResidueContainer::ResidueIt & b )
	{
		return a._system == b._system && a._index == b._index;
	}
	bool operator!=( const ResidueContainer::ResidueIt & a, const ResidueContainer::ResidueIt & b )
	{
		return a._system != b._system || a._index != b._index;
	}

	void ResidueContainer::ResidueIt::_getValid()
	{
		while ( _index != _end && _system->getResidue( _index ) == nullptr )
		{
			_index++;
		}
	}

	ResidueContainer::ResidueContainer(
		System * const p_system,
		const size_t   p_firstResidueIndex,
		const size_t   p_residueCount
	) : _system( p_system ), _firstResidueIndex( p_firstResidueIndex ), _residueCount( p_residueCount )
	{
	}

	ResidueContainer::ResidueIt ResidueContainer::begin()
	{
		return ResidueContainer::ResidueIt( _system, _firstResidueIndex, _firstResidueIndex + _residueCount );
	}
	ResidueContainer::ResidueIt ResidueContainer::end()
	{
		return ResidueContainer::ResidueIt(
			_system, _firstResidueIndex + _residueCount, _firstResidueIndex + _residueCount
		);
	}
} // namespace VTX::App::Component::Chemistry::Iterator
