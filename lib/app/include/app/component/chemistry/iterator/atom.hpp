#ifndef __VTX_APP_COMPONENT_CHEMISTRY_ITERATOR_ATOM__
#define __VTX_APP_COMPONENT_CHEMISTRY_ITERATOR_ATOM__

#include "app/component/chemistry/_fwd.hpp"
#include <cstddef>
#include <util/types.hpp>

namespace VTX::App::Component::Chemistry::Iterator
{
	struct AtomContainer
	{
		struct AtomIt
		{
			AtomIt( System * const p_molecule, const atom_index_t p_index, const atom_index_t p_end );

			Atom & operator*() const;
			Atom * operator->();

			friend bool operator==( const AtomIt & a, const AtomIt & b );
			friend bool operator!=( const AtomIt & a, const AtomIt & b );

			// Prefix increment
			AtomIt & operator++();

			// Postfix increment
			AtomIt operator++( int );

		  private:
			System * const   _molecule;
			atom_index_t	   _index;
			const atom_index_t _end;

			void _getValid();
		};

		AtomContainer(
			System * const   p_molecule,
			const atom_index_t p_firstAtomIndex,
			const atom_index_t p_atomCount
		);

		AtomIt begin();
		AtomIt end();

	  private:
		System * const   _molecule;
		const atom_index_t _firstAtomIndex;
		const atom_index_t _atomCount;
	};
} // namespace VTX::App::Component::Chemistry::Iterator
#endif
