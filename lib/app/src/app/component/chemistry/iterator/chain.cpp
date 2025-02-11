#include "app/component/chemistry/iterator/chain.hpp"
#include "app/component/chemistry/system.hpp"

namespace VTX::App::Component::Chemistry::Iterator
{
	ChainContainer::ChainIt::ChainIt( System * const p_system, const size_t p_index, const size_t p_end ) :
		_system( p_system ), _index( p_index ), _end( p_end )
	{
		_getValid();
	}

	Chain & ChainContainer::ChainIt::operator*() const { return *( _system->getChain( _index ) ); }
	Chain * ChainContainer::ChainIt::operator->() { return _system->getChain( _index ); }

	// Prefix increment
	ChainContainer::ChainIt & ChainContainer::ChainIt::operator++()
	{
		_index++;
		_getValid();

		return *this;
	}

	// Postfix increment
	ChainContainer::ChainIt ChainContainer::ChainIt::operator++( int )
	{
		ChainIt tmp = ChainIt( _system, _index, _end );

		_index++;
		_getValid();

		return tmp;
	}

	bool operator==( const ChainContainer::ChainIt & a, const ChainContainer::ChainIt & b )
	{
		return a._system == b._system && a._index == b._index;
	}
	bool operator!=( const ChainContainer::ChainIt & a, const ChainContainer::ChainIt & b )
	{
		return a._system != b._system || a._index != b._index;
	}

	void ChainContainer::ChainIt::_getValid()
	{
		while ( _index != _end && _system->getChain( _index ) == nullptr )
		{
			_index++;
		}
	}

	ChainContainer::ChainContainer(
		System * const p_system,
		const size_t   p_firstChainIndex,
		const size_t   p_ChainCount
	) : _system( p_system ), _firstChainIndex( p_firstChainIndex ), _chainCount( p_ChainCount )
	{
	}

	ChainContainer::ChainIt ChainContainer::begin()
	{
		return ChainContainer::ChainIt( _system, _firstChainIndex, _firstChainIndex + _chainCount );
	}
	ChainContainer::ChainIt ChainContainer::end()
	{
		return ChainContainer::ChainIt( _system, _firstChainIndex + _chainCount, _firstChainIndex + _chainCount );
	}
} // namespace VTX::App::Component::Chemistry::Iterator
