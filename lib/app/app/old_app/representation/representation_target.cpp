#include "representation_target.hpp"

namespace VTX
{
	namespace Representation
	{
		void RepresentationTarget::generate()
		{
			_mapToRangeArrays( _atomsMap, _atoms );
			_mapToRangeElements( _bondsMap, _bonds );
			_mapToRangeElements( _ribbonsMap, _ribbons );

			for ( auto & [ key, val ] : _trianglesSESMap )
			{
				assert( _trianglesSES.find( key ) != _trianglesSES.end() );
				_mapToRangeElements( val, _trianglesSES[ key ] );
			}

			_isGenerated = true;
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

		void RepresentationTarget::_mapToRangeArrays( TargetRangeMap & p_map, TargetRangeArrays & p_rangeArrays )
		{
			// Already compiled.
			assert( p_rangeArrays.indices.size() == 0 );
			assert( p_rangeArrays.counts.size() == 0 );

			for ( const TargetRangeMapPair & pair : p_map )
			{
				p_rangeArrays.indices.push_back( pair.first );
				p_rangeArrays.counts.push_back( pair.second );
			}

			p_map = TargetRangeMap();
		}

		void RepresentationTarget::_mapToRangeElements( TargetRangeMap & p_map, TargetRangeElements & p_rangeElements )
		{
			// Already compiled.
			assert( p_rangeElements.indices.size() == 0 );
			assert( p_rangeElements.counts.size() == 0 );

			for ( const TargetRangeMapPair & pair : p_map )
			{
				p_rangeElements.indices.push_back( (void *)( pair.first * sizeof( uint ) ) );
				p_rangeElements.counts.push_back( pair.second );
			}

			p_map = TargetRangeMap();
		}
	} // namespace Representation
} // namespace VTX
