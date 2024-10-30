#ifndef __VTX_APP_COMPONENT_CHEMISTRY_ITERATOR_RESIDUE__
#define __VTX_APP_COMPONENT_CHEMISTRY_ITERATOR_RESIDUE__

#include "app/component/chemistry/_fwd.hpp"
#include <cstddef>

namespace VTX::App::Component::Chemistry::Iterator
{
	struct ResidueContainer
	{
		struct ResidueIt
		{
			ResidueIt( System * const p_system, const size_t p_index, const size_t p_end );

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
			size_t			 _index;
			const size_t	 _end;

			void _getValid();
		};

		ResidueContainer( System * const p_system, const size_t p_firstResidueIndex, const size_t p_residueCount );

		ResidueIt begin();
		ResidueIt end();

	  private:
		System * const _system;
		const size_t	 _firstResidueIndex;
		const size_t	 _residueCount;
	};
} // namespace VTX::App::Component::Chemistry::Iterator
#endif
