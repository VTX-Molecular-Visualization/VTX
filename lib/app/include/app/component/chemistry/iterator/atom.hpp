#ifndef __VTX_APP_COMPONENT_CHEMISTRY_ITERATOR_ATOM__
#define __VTX_APP_COMPONENT_CHEMISTRY_ITERATOR_ATOM__

#include "app/component/chemistry/_fwd.hpp"

namespace VTX::App::Component::Chemistry::Iterator
{
	struct AtomContainer
	{
		struct AtomIt
		{
			AtomIt( Molecule * const p_molecule, const size_t p_index, const size_t p_end );

			Atom & operator*() const;
			Atom * operator->();

			friend bool operator==( const AtomIt & a, const AtomIt & b );
			friend bool operator!=( const AtomIt & a, const AtomIt & b );

			// Prefix increment
			AtomIt & operator++();

			// Postfix increment
			AtomIt operator++( int );

		  private:
			Molecule * const _molecule;
			size_t			 _index;
			const size_t	 _end;

			void _getValid();
		};

		AtomContainer( Molecule * const p_molecule, const size_t p_firstAtomIndex, const size_t p_atomCount );

		AtomIt begin();
		AtomIt end();

	  private:
		Molecule * const _molecule;
		const size_t	 _firstAtomIndex;
		const size_t	 _atomCount;
	};
} // namespace VTX::App::Component::Chemistry::Iterator
#endif
