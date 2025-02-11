#ifndef __VTX_APP_COMPONENT_CHEMISTRY_ITERATOR_CHAIN__
#define __VTX_APP_COMPONENT_CHEMISTRY_ITERATOR_CHAIN__

#include "app/component/chemistry/_fwd.hpp"
#include <cstddef>

namespace VTX::App::Component::Chemistry::Iterator
{
	struct ChainContainer
	{
		struct ChainIt
		{
			ChainIt( System * const p_system, const size_t p_index, const size_t p_end );

			Chain & operator*() const;
			Chain * operator->();

			friend bool operator==( const ChainIt & a, const ChainIt & b );
			friend bool operator!=( const ChainIt & a, const ChainIt & b );

			// Prefix increment
			ChainIt & operator++();

			// Postfix increment
			ChainIt operator++( int );

		  private:
			System * const _system;
			size_t			 _index;
			const size_t	 _end;

			void _getValid();
		};

		ChainContainer( System * const p_system, const size_t p_firstChainIndex, const size_t p_chainCount );

		ChainIt begin();
		ChainIt end();

	  private:
		System * const _system;
		const size_t	 _firstChainIndex;
		const size_t	 _chainCount;
	};
} // namespace VTX::App::Component::Chemistry::Iterator
#endif
