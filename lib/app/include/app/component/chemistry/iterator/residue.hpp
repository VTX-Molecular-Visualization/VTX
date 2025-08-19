#ifndef __VTX_APP_COMPONENT_CHEMISTRY_ITERATOR_RESIDUE__
#define __VTX_APP_COMPONENT_CHEMISTRY_ITERATOR_RESIDUE__

#include "app/component/chemistry/_fwd.hpp"
#include <cstddef>
#include <util/types.hpp>

namespace VTX::App::Component::Chemistry::Iterator
{
	struct ResidueContainer
	{
		struct ResidueIt
		{
			ResidueIt( System * const p_system, const Index p_index, const Index p_end );

			Residue & operator*() const;
			Residue * operator->();

			friend bool operator==( const ResidueIt & a, const ResidueIt & b );
			friend bool operator!=( const ResidueIt & a, const ResidueIt & b );

			// Prefix increment
			ResidueIt & operator++();

			// Postfix increment
			ResidueIt operator++( int );

		  private:
			System * const _system;
			Index		   _index;
			const Index	   _end;

			void _getValid();
		};

		ResidueContainer( System * const p_system, const Index p_firstResidueIndex, const Index p_residueCount );

		ResidueIt begin();
		ResidueIt end();

	  private:
		System * const _system;
		const Index	   _firstResidueIndex;
		const Index	   _residueCount;
	};
} // namespace VTX::App::Component::Chemistry::Iterator
#endif
