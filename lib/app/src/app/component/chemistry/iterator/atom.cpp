#include "app/component/chemistry/iterator/atom.hpp"
#include "app/component/chemistry/system.hpp"

namespace VTX::App::Component::Chemistry::Iterator
{
	AtomContainer::AtomIt::AtomIt( System * const p_molecule, const atom_index_t p_index, const atom_index_t p_end ) :
		_molecule( p_molecule ), _index( p_index ), _end( p_end )
	{
		_getValid();
	}

	Atom & AtomContainer::AtomIt::operator*() const { return *( _molecule->getAtom( _index ) ); }
	Atom * AtomContainer::AtomIt::operator->() { return _molecule->getAtom( _index ); }

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
		AtomIt tmp = AtomIt( _molecule, _index, _end );

		_index++;
		_getValid();

		return tmp;
	}

	bool operator==( const AtomContainer::AtomIt & a, const AtomContainer::AtomIt & b )
	{
		return a._molecule == b._molecule && a._index == b._index;
	}
	bool operator!=( const AtomContainer::AtomIt & a, const AtomContainer::AtomIt & b )
	{
		return a._molecule != b._molecule || a._index != b._index;
	}

	void AtomContainer::AtomIt::_getValid()
	{
		while ( _index != _end && _molecule->getAtom( _index ) == nullptr )
		{
			_index++;
		}
	}

	AtomContainer::AtomContainer(
		System * const	   p_molecule,
		const atom_index_t p_firstAtomIndex,
		const atom_index_t p_atomCount
	) : _molecule( p_molecule ), _firstAtomIndex( p_firstAtomIndex ), _atomCount( p_atomCount )
	{
	}

	AtomContainer::AtomIt AtomContainer::begin()
	{
		return AtomContainer::AtomIt( _molecule, _firstAtomIndex, _firstAtomIndex + _atomCount );
	}
	AtomContainer::AtomIt AtomContainer::end()
	{
		return AtomContainer::AtomIt( _molecule, _firstAtomIndex + _atomCount, _firstAtomIndex + _atomCount );
	}
} // namespace VTX::App::Component::Chemistry::Iterator
