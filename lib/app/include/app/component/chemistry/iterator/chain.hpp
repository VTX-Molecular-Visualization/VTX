#ifndef __VTX_APP_COMPONENT_CHEMISTRY_ITERATOR_CHAIN__
#define __VTX_APP_COMPONENT_CHEMISTRY_ITERATOR_CHAIN__

#include "app/component/chemistry/_fwd.hpp"
#include <cstddef>
#include <util/types.hpp>

namespace VTX::App::Component::Chemistry::Iterator
{
	struct ChainContainer
	{
		struct ChainIt
		{
			ChainIt( System * const p_system, const Index p_index, const Index p_end );

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
			Index		   _index;
			const Index	   _end;

			void _getValid();
		};

		ChainContainer( System * const p_system, const Index p_firstChainIndex, const Index p_chainCount );

		ChainIt begin();
		ChainIt end();

	  private:
		System * const _system;
		const Index	   _firstChainIndex;
		const Index	   _chainCount;
	};
} // namespace VTX::App::Component::Chemistry::Iterator
#endif
