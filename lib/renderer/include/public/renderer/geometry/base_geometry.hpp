#ifndef __VTX_RENDERER_GEOMETRY_BASE_GEOMETRY__
#define __VTX_RENDERER_GEOMETRY_BASE_GEOMETRY__

#include "renderer/system_data.hpp"
#include <map>
#include <util/exceptions.hpp>
#include <util/logger.hpp>
#include <util/math/range_list.hpp>
#include <util/type_traits.hpp>
#include <util/types.hpp>

namespace VTX::Renderer::Geometry
{
	/**
	 * @brief Type aliases.
	 */
	using IndexRange		= Util::Math::Range<Index>;
	using IndexRangeList	= Util::Math::RangeList<Index>;
	using MapUIDRange		= std::map<SystemUID, IndexRange>;
	using MapUIDRangeList	= std::map<SystemUID, IndexRangeList>;
	using DrawRangeArray	= Desc::DrawCall::RangeArrays;
	using DrawRangeElements = Desc::DrawCall::RangeElements;

	/**
	 * @brief If more than this number of consecutive items are not visible, split draw calls.
	 * Otherwise, use bool mask to skip them in shader.
	 */
	constexpr size_t RANGE_CHUNK_SIZE = 10;

	/**
	 * @brief Base geometry struct to handle and build draw ranges.
	 */
	template<typename DR>
	class BaseGeometry
	{
	  public:
		/**
		 * @brief Current size to draw (before applying anything).
		 */
		Index size;

		/**
		 * @brief Push a range.
		 */
		template<typename T>
		void addRange( const SystemUID p_uid, const T p_count )
		{
			size_t count = size + p_count;
			if ( count > TypeMax<Index> )
			{
				throw GraphicException( "Total geometry count exceeds maximum supported value." );
			}

			Index countIndex = static_cast<Index>( count );
			_ranges[ p_uid ] = IndexRange { size, countIndex };
			size			 = countIndex;
		}

		/**
		 * @brief Mask of ranges to not draw per system (local indexes).
		 */
		MapUIDRangeList visibilityMask;
		MapUIDRangeList representationMask;

		/**
		 * @brief Compiled draw ranges for GPU calls.
		 */
		DR drawRanges;

		/**
		 * @brief Build GPU draw ranges.
		 */
		void buildDrawRanges()
		{
			drawRanges.firsts.clear();
			drawRanges.counts.clear();

			IndexRangeList allRanges;
			for ( const auto & [ uid, range ] : _ranges )
			{
				// Range as list.
				IndexRangeList rangeList( range );

				IndexRangeList visibillityToRemove	  = visibilityMask[ uid ];
				IndexRangeList representationToRemove = representationMask[ uid ];

				const Index first = _ranges[ uid ].first;

				// Remove masked ranges.
				for ( auto it = visibillityToRemove.rangeBegin(); it != visibillityToRemove.rangeEnd(); ++it )
				{
					// If > threshold.
					if ( it->getCount() >= RANGE_CHUNK_SIZE )
					{
						// Shift to global index.
						it->shiftInPlace( first );
						rangeList.removeRange( *it );
					}
				}
				for ( auto it = representationToRemove.rangeBegin(); it != representationToRemove.rangeEnd(); ++it )
				{
					it->shiftInPlace( first );
					rangeList.removeRange( *it );
				}

				allRanges.mergeInPlace( rangeList );
			}

			// TODO: make this API-agnostic.
			if constexpr ( std::is_same_v<DR, DrawRangeArray> )
			{
				allRanges.toStdVectorsFirstCount(
					IndexRangeList::VectorParam<int32_t> { drawRanges.firsts },
					IndexRangeList::VectorParam<uint32_t> { drawRanges.counts }
				);
			}
			else if constexpr ( std::is_same_v<DR, DrawRangeElements> )
			{
				allRanges.toStdVectorsFirstCount(
					IndexRangeList::VectorParam<uintptr_t> { drawRanges.firsts, sizeof( Index ) },
					IndexRangeList::VectorParam<uint32_t> { drawRanges.counts }
				);
			}
			else
			{
				static_assert( always_false_v<DR>, "Unsupported draw range type" );
			}

			VTX_DEBUG( "Built draw ranges: {}", allRanges.rangeCount() );
		}

		IndexRange range( const SystemUID p_uid ) const
		{
			assert( _ranges.contains( p_uid ) );

			return _ranges[ p_uid ];
		}

	  protected:
		/**
		 * @brief Range to draw per system (global indexes).
		 */
		mutable MapUIDRange _ranges;
	};
} // namespace VTX::Renderer::Geometry

#endif
