#ifndef __VTX_RENDERER_GEOMETRY_BASE_GEOMETRY__
#define __VTX_RENDERER_GEOMETRY_BASE_GEOMETRY__

#include "renderer/descriptors.hpp"
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
	using IndexRange	  = Util::Math::Range<Index>;
	using IndexRangeList  = Util::Math::RangeList<Index>;
	using MapUIDRange	  = std::map<SystemUID, IndexRange>;
	using MapUIDRangeList = std::map<SystemUID, IndexRangeList>;

	/**
	 * @brief If more than this number of consecutive items are not visible, split draw calls.
	 * Otherwise, use bool mask to skip them in shader.
	 */
	constexpr size_t RANGE_CHUNK_SIZE = 10;

	/**
	 * @brief Base geometry struct to handle and build draw ranges.
	 */
	class BaseGeometry
	{
	  public:
		/**
		 * @brief Current size to draw (before applying anything).
		 */
		Index size = 0;

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
		// Desc::DrawCall::Ranges drawRanges;
		uint32_t count;

		/**
		 * @brief Build GPU draw ranges.
		 */
		void buildDrawRanges()
		{
			count = 0;

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

			// TODO: push to GPU.
			/*
			allRanges.toStdVectorsFirstCount(
				IndexRangeList::VectorParam<uint32_t> { drawRanges.firsts, sizeof( Index ) },
					IndexRangeList::VectorParam<uint32_t> { drawRanges.counts }
			);
			*/

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
