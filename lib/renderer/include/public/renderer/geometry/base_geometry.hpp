#ifndef __VTX_RENDERER_GEOMETRY_BASE_GEOMETRY__
#define __VTX_RENDERER_GEOMETRY_BASE_GEOMETRY__

#include <map>
#include <util/logger.hpp>
#include <util/math/range_list.hpp>
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
	constexpr size_t VISIBILITY_CHUNK_THRESHOLD = 10;

	/**
	 * @brief Base geometry struct to handle and build draw ranges.
	 */
	template<typename DR>
	struct BaseGeometry
	{
		/**
		 * @brief Range to draw per system.
		 */
		MapUIDRange ranges;

		/**
		 * @brief Mask of ranges to not draw per system (local index).
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
			for ( const auto & [ uid, range ] : ranges )
			{
				// Range as list.
				IndexRangeList rangeList( range );

				// Remove masked ranges.
				const Index first = ranges[ uid ].first;
				for ( IndexRange m : visibilityMask[ uid ] )
				{
					// Shift to global index.
					m.shift( first );
					rangeList.removeRange( m );
				}
				for ( IndexRange m : representationMask[ uid ] )
				{
					m.shift( first );
					rangeList.removeRange( m );
				}

				allRanges.mergeInPlace( rangeList );
			}

			allRanges.toStdVectorsFirstCount( drawRanges.firsts, drawRanges.counts );

			VTX_DEBUG( "Built draw ranges: {} ranges, {} items", drawRanges.firsts.size(), allRanges.rangeCount() );
		}
	};
} // namespace VTX::Renderer::Geometry

#endif
