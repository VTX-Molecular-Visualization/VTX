#include "app/component/chemistry/iterator/atom.hpp"
#include "app/component/chemistry/system.hpp"

namespace VTX::App::Component::Chemistry::Iterator
{
	AtomContainer::AtomIt::AtomIt( System * const p_system, const atom_index_t p_index, const atom_index_t p_end ) :
		_system( p_system ), _index( p_index ), _end( p_end )
	{
		_getValid();
	}

	Atom & AtomContainer::AtomIt::operator*() const { return *( _system->getAtom( _index ) ); }
	Atom * AtomContainer::AtomIt::operator->() { return _system->getAtom( _index ); }

	// Prefix increment
	AtomContainer::AtomIt & AtomContainer::AtomIt::operator++()
	{
		_index++;
		_getValid();

		return *this;
	}

	// Postfix increment
	AtomContainer::AtomIt AtomContainer::AtomIt::operator++( int )
	{
		AtomIt tmp = AtomIt( _system, _index, _end );

		_index++;
		_getValid();

		return tmp;
	}

	bool operator==( const AtomContainer::AtomIt & a, const AtomContainer::AtomIt & b )
	{
		return a._system == b._system && a._index == b._index;
	}
	bool operator!=( const AtomContainer::AtomIt & a, const AtomContainer::AtomIt & b )
	{
		return a._system != b._system || a._index != b._index;
	}

	void AtomContainer::AtomIt::_getValid()
	{
		while ( _index != _end && _system->getAtom( _index ) == nullptr )
		{
			_index++;
		}
	}

	AtomContainer::AtomContainer(
		System * const	   p_system,
		const atom_index_t p_firstAtomIndex,
		const atom_index_t p_atomCount
	) : _system( p_system ), _firstAtomIndex( p_firstAtomIndex ), _atomCount( p_atomCount )
	{
	}

	AtomContainer::AtomIt AtomContainer::begin()
	{
		return AtomContainer::AtomIt( _system, _firstAtomIndex, _firstAtomIndex + _atomCount );
	}
	AtomContainer::AtomIt AtomContainer::end()
	{
		return AtomContainer::AtomIt( _system, _firstAtomIndex + _atomCount, _firstAtomIndex + _atomCount );
	}
} // namespace VTX::App::Component::Chemistry::Iterator
