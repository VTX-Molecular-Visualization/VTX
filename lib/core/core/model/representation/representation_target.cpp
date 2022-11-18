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

		void RepresentationTarget::generate()
		{
			_generateArrays( (const TargetRangeMap **)( &_atomsMap ), _atoms );
			_generateElements( (const TargetRangeMap **)( &_bondsMap ), _bonds );
			_generateElements( (const TargetRangeMap **)( &_ribbonsMap ), _ribbons );
			_generateElements( (const TargetRangeMap **)( &_trianglesSESMap ), _trianglesSES );
		}

		void RepresentationTarget::_generateArrays( const TargetRangeMap ** const p_mapPtrPtr,
													TargetRangeArrays &			  p_rangeArrays )
		{
			const TargetRangeMap *& mapPtr = *p_mapPtrPtr;

			if ( mapPtr != nullptr )
			{
				_mapToRangeArrays( *mapPtr, p_rangeArrays );
				delete mapPtr;
				mapPtr = nullptr;
			}
		}

		void RepresentationTarget::_generateElements( const TargetRangeMap ** const p_mapPtrPtr,
													  TargetRangeElements &			p_rangeElements )
		{
			const TargetRangeMap *& mapPtr = *p_mapPtrPtr;

			if ( mapPtr != nullptr )
			{
				_mapToRangeElements( *mapPtr, p_rangeElements );
				delete mapPtr;
				mapPtr = nullptr;
			}
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
