#ifndef __VTX_RENDERER_GEOMETRY_BASE_GEOMETRY__
#define __VTX_RENDERER_GEOMETRY_BASE_GEOMETRY__

#include "renderer/descriptors.hpp"
#include "renderer/resource_handler.hpp"
#include "renderer/system_data.hpp"
#include "renderer/types.hpp"
#include <map>
#include <util/exceptions.hpp>
#include <util/logger.hpp>
#include <util/math/bitset.hpp>
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
		// MapUIDRangeList visibilityMask;
		// MapUIDRangeList representationMask;

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
			for ( const auto & [ uid, data ] : _resources )
			{
				size += data.range.getCount();
			}

			if ( indiceBuffer )
			{
				p_context.setPipelineBuffer<Indice>( *indiceBuffer, size == 0 ? 1 : size );
			}
		}

		/**
		 * @brief Upload index buffer data.
		 */
		void uploadIndexes( Context::ContextWrapper & p_context, const Desc::Handle p_handle )
		{
			assert( indiceBuffer );

			p_context.setPipelineBuffer<Indice>(
				*indiceBuffer, _resources[ p_handle ].indices, _resources[ p_handle ].range.first
			);
		}

		/**
		 * @brief Build GPU draw commands from the current ranges and masks.
		 */
		[[nodiscard]] std::vector<Desc::DrawIndirectCommand> toDrawIndirectCommands()
		{
			std::vector<Desc::DrawIndirectCommand> commands;

			for ( const auto & [ uid, data ] : _resources )
			{
				commands.emplace_back(
					Desc::DrawIndirectCommand { data.range.getCount(), 1, data.range.getFirst(), 0 }
				);
			}

			count = static_cast<uint32_t>( commands.size() );

			return commands;
		}

		/**
		 * @brief Build GPU draw indexed commands from the current ranges and masks.
		 */
		[[nodiscard]] std::vector<Desc::DrawIndexedIndirectCommand> toDrawIndexedIndirectCommands()
		{
			std::vector<Desc::DrawIndexedIndirectCommand> commands;
			int											  baseVertex = 0;

			for ( const auto & [ uid, data ] : _resources )
			{
				commands.emplace_back(
					Desc::DrawIndexedIndirectCommand {
						static_cast<uint32_t>( data.indices.size() ), 1, data.range.getFirst(), baseVertex, 0 }
				);
				baseVertex += data.vertexCount;
			}

			count = static_cast<uint32_t>( commands.size() );

			return commands;
		}

		Index size( const Desc::Handle p_handle ) const
		{
			assert( _resources.contains( p_handle ) );

			return _resources.at( p_handle ).range.getCount();
		}

	  protected:
		/**
		 * @brief Data per system.
		 */
		struct Data
		{
			IndexRange			range;
			Index				vertexCount;
			std::vector<Indice> indices;
		};

		/**
		 * @brief Push a range.
		 */
		void _addRange( const Desc::Handle p_handle, const Index p_countIndices, const Index p_countVertex )
		{
			size_t count = _size + p_countIndices;
			if ( count > TypeMax<Index> )
			{
				throw GraphicException( "Total geometry count exceeds maximum supported value." );
			}

			Index countIndex = static_cast<Index>( count );
			_resources.emplace( p_handle, Data { IndexRange { _size, countIndex }, p_countVertex } );
			_size = countIndex;
		}

		/**
		 * @brief Access indices.
		 */
		std::vector<Indice> & _indices( const Desc::Handle p_handle )
		{
			assert( _resources.contains( p_handle ) );

			return _resources[ p_handle ].indices;
		}

	  private:
		/**
		 * @brief Resources.
		 */
		std::unordered_map<Desc::Handle, Data> _resources;

		/**
		 * @brief Current size to draw (before applying anything).
		 */
		Index _size = 0;
	};
} // namespace VTX::Renderer::Geometry

#endif
