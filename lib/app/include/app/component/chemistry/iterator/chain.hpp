#ifndef __VTX_APP_COMPONENT_CHEMISTRY_ITERATOR_CHAIN__
#define __VTX_APP_COMPONENT_CHEMISTRY_ITERATOR_CHAIN__

#include "app/component/chemistry/_fwd.hpp"

namespace VTX::App::Component::Chemistry::Iterator
{
	struct ChainContainer
	{
		struct ChainIt
		{
			ChainIt( Molecule * const p_molecule, const size_t p_index, const size_t p_end );

			Chain & operator*() const;
			Chain * operator->();

			friend bool operator==( const ChainIt & a, const ChainIt & b );
			friend bool operator!=( const ChainIt & a, const ChainIt & b );

			// Prefix increment
			ChainIt & operator++();

			// Postfix increment
			ChainIt operator++( int );

		  private:
			Molecule * const _molecule;
			size_t			 _index;
			const size_t	 _end;

			void _getValid();
		};

		ChainContainer( Molecule * const p_molecule, const size_t p_firstChainIndex, const size_t p_chainCount );

		ChainIt begin();
		ChainIt end();

	  private:
		Molecule * const _molecule;
		const size_t	 _firstChainIndex;
		const size_t	 _chainCount;
	};
} // namespace VTX::App::Component::Chemistry::Iterator
#endif
