#include "representation_target.hpp"

namespace VTX
{
	namespace Representation
	{
		RepresentationTarget ::~RepresentationTarget()
		{
			if ( _atomsMap == nullptr )
			{
				delete _atomsMap;
			}
			if ( _bondsMap == nullptr )
			{
				delete _bondsMap;
			}
			if ( _ribbonsMap == nullptr )
			{
				delete _ribbonsMap;
			}
			if ( _trianglesSESMap == nullptr )
			{
				delete _trianglesSESMap;
			}
		}

		void RepresentationTarget::compile()
		{
			_mapToRangeArrays( *_atomsMap, _atoms );
			_mapToRangeElements( *_bondsMap, _bonds );
			_mapToRangeElements( *_ribbonsMap, _ribbons );
			_mapToRangeElements( *_trianglesSESMap, _trianglesSES );

			delete _atomsMap;
			delete _bondsMap;
			delete _ribbonsMap;
			delete _trianglesSESMap;

			_atomsMap		 = nullptr;
			_bondsMap		 = nullptr;
			_ribbonsMap		 = nullptr;
			_trianglesSESMap = nullptr;
		}

		void RepresentationTarget::_append( TargetRangeMap & p_range, const uint p_indice, const uint p_count )
		{
			if ( p_count <= 0 )
			{
				return;
			}

			for ( std::map<uint, uint>::iterator it = p_range.begin(); it != p_range.end(); it++ )
			{
				const uint itFirstIndex = it->first;
				uint &	   itCount		= it->second;
				const uint itNextIndex	= itFirstIndex + itCount;

				// Already in buffer.
				if ( itFirstIndex <= p_indice && p_indice < itNextIndex )
				{
					return;
				}

				if ( itNextIndex == p_indice )
				{
					itCount += p_count;

					// If new last index join the next range, fusion it.
					std::map<uint, uint>::iterator nextIt = it;
					nextIt++;
					if ( nextIt != p_range.end() && it->second == nextIt->first )
					{
						itCount += nextIt->second;
						p_range.erase( nextIt );
					}

					return;
				}
			}

			p_range.emplace( p_indice, p_count );
		}

		void RepresentationTarget::_mapToRangeArrays( const TargetRangeMap & p_map, TargetRangeArrays & p_rangeArrays )
		{
			// Already compiled.
			assert( p_rangeArrays.indices.size() == 0 );
			assert( p_rangeArrays.counts.size() == 0 );

			for ( const TargetRangeMapPair & pair : p_map )
			{
				p_rangeArrays.indices.push_back( pair.first );
				p_rangeArrays.counts.push_back( pair.second );
			}
		}

		void RepresentationTarget::_mapToRangeElements( const TargetRangeMap & p_map,
														TargetRangeElements &  p_rangeElements )
		{
			// Already compiled.
			assert( p_rangeElements.indices.size() == 0 );
			assert( p_rangeElements.counts.size() == 0 );

			for ( const TargetRangeMapPair & pair : p_map )
			{
				p_rangeElements.indices.push_back( (void *)( pair.first * sizeof( uint ) ) );
				p_rangeElements.counts.push_back( pair.second );
			}
		}
	} // namespace Representation
} // namespace VTX
