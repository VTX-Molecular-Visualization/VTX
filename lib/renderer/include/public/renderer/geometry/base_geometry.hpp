#ifndef __VTX_RENDERER_GEOMETRY_BASE_GEOMETRY__
#define __VTX_RENDERER_GEOMETRY_BASE_GEOMETRY__

#include "renderer/descriptors.hpp"
#include "renderer/system_data.hpp"
#include "renderer/types.hpp"
#include <map>
#include <util/exceptions.hpp>
#include <util/logger.hpp>
#include <util/math/range_list.hpp>
#include <util/type_traits.hpp>
#include <util/types.hpp>

namespace VTX::Renderer::Geometry
{

	/**
	 * @brief If more than this number of consecutive items are not visible, split draw calls.
	 * Otherwise, use bool mask to skip them in shader.
	 */
	constexpr size_t RANGE_CHUNK_SIZE = 10;

	/**
	 * @brief Base geometry struct to handle and build draw ranges.
	 */
	class BaseGeometry : public Desc::Geometry
	{
	  public:
		/**
		 * @brief Mask of ranges to not draw per system (local indexes).
		 */
		MapUIDRangeList visibilityMask;
		MapUIDRangeList representationMask;

		/**
		 * @brief Compiled draw ranges count for GPU calls.
		 */
		uint32_t count = 0;

		/**
		 * @brief Resize whole layout.
		 */
		inline void resize( Context::ContextWrapper & p_context )
		{
			Index size = 0;
			for ( const auto & [ uid, range ] : _ranges )
			{
				size += range.getCount();
			}

			if ( indexBuffer )
			{
				p_context.setPipelineBuffer<uint32_t>( *indexBuffer, size );
			}
		}

		/**
		 * @brief Upload index buffer data.
		 */
		void uploadIndexes(
			Context::ContextWrapper & p_context,
			const SystemUID			  p_uid,
			std::span<const uint32_t> p_indexes
		)
		{
			if ( indexBuffer )
			{
				p_context.setPipelineBuffer<uint32_t>( *indexBuffer, p_indexes, _ranges[ p_uid ].first );
			}
			else
			{
				throw GraphicException( "This geometry does not support index buffer upload." );
			}
		}

		/**
		 * @brief Build GPU draw commands from the current ranges and masks.
		 */
		[[nodiscard]] std::vector<Desc::DrawIndirectCommand> toDrawIndirectCommands()
		{
			IndexRangeList						   allRanges = _buildDrawRanges();
			std::vector<Desc::DrawIndirectCommand> commands;

			for ( auto it = allRanges.rangeBegin(); it != allRanges.rangeEnd(); ++it )
			{
				commands.emplace_back( Desc::DrawIndirectCommand { it->getCount(), 1, it->getFirst(), 0 } );
			}

			count = static_cast<uint32_t>( commands.size() );

			return commands;
		}

		/**
		 * @brief Build GPU draw indexed commands from the current ranges and masks.
		 */
		[[nodiscard]] std::vector<Desc::DrawIndexedIndirectCommand> toDrawIndexedIndirectCommands()
		{
			IndexRangeList								  allRanges = _buildDrawRanges();
			std::vector<Desc::DrawIndexedIndirectCommand> commands;

			for ( auto it = allRanges.rangeBegin(); it != allRanges.rangeEnd(); ++it )
			{
				commands.emplace_back( Desc::DrawIndexedIndirectCommand { it->getCount(), 1, it->getFirst(), 0, 0 } );
			}

			count = static_cast<uint32_t>( commands.size() );

			return commands;
		}

		Index size( const SystemUID p_uid ) const
		{
			assert( _ranges.contains( p_uid ) );

			return _ranges[ p_uid ].getCount();
		}

	  protected:
		/**
		 * @brief Range to draw per system (global indexes).
		 */
		mutable MapUIDRange _ranges;

		/**
		 * @brief Current size to draw (before applying anything).
		 */
		Index _size = 0;

		/**
		 * @brief Push a range.
		 */
		template<typename T>
		void _addRange( const SystemUID p_uid, const T p_count )
		{
			size_t count = _size + p_count;
			if ( count > TypeMax<Index> )
			{
				throw GraphicException( "Total geometry count exceeds maximum supported value." );
			}

			Index countIndex = static_cast<Index>( count );
			_ranges[ p_uid ] = IndexRange { _size, countIndex };
			_size			 = countIndex;
		}

	  private:
		/**
		 * @brief Build GPU draw ranges.
		 */
		[[nodiscard]] IndexRangeList _buildDrawRanges()
		{
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

			VTX_DEBUG( "Built draw ranges: {}", allRanges.rangeCount() );

			return allRanges;
		}
	};
} // namespace VTX::Renderer::Geometry

#endif
