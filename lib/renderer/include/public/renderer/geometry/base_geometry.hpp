#ifndef __VTX_RENDERER_GEOMETRY_BASE_GEOMETRY__
#define __VTX_RENDERER_GEOMETRY_BASE_GEOMETRY__

#include "renderer/context/context_wrapper.hpp"
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
	 * @brief Base geometry struct to handle and build draw ranges.
	 */
	class BaseGeometry : public Desc::Geometry
	{
	  public:
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
				p_context.setBuffer<Indice>( { *indiceBuffer }, size == 0 ? 1 : size );
			}
		}

		/**
		 * @brief Clear all geometry ranges and cached indices.
		 */
		void clear()
		{
			_resources.clear();
			chunks.clear();
			_size = 0;
		}

		/**
		 * @brief Upload index buffer data.
		 */
		void uploadIndexes( Context::ContextWrapper & p_context, const Desc::Handle p_handle )
		{
			assert( indiceBuffer );

			p_context.setBuffer<Indice>(
				{ *indiceBuffer }, _resources[ p_handle ].indices, _resources[ p_handle ].range.first
			);
		}

		/**
		 * @brief Build GPU draw commands from the current ranges and masks.
		 */
		[[nodiscard]] std::vector<Desc::DrawIndirectRecord> toDrawIndirectCommands()
		{
			std::vector<Desc::DrawIndirectRecord> records;

			for ( const auto & [ uid, data ] : _resources )
			{
				records.emplace_back(
					Desc::DrawIndirectRecord {
						Desc::DrawIndirectCommand { data.range.getCount(), 1, data.range.getFirst(), 0 },
						static_cast<uint32_t>( uid ) }
				);
			}

			return records;
		}

		/**
		 * @brief Build GPU draw indexed commands from the current ranges and masks.
		 */
		[[nodiscard]] std::vector<Desc::DrawIndexedIndirectRecord> toDrawIndexedIndirectCommands()
		{
			std::vector<Desc::DrawIndexedIndirectRecord> records;
			int											 baseVertex = 0;

			for ( const auto & [ uid, data ] : _resources )
			{
				records.emplace_back(
					Desc::DrawIndexedIndirectRecord {
						Desc::DrawIndexedIndirectCommand {
							static_cast<uint32_t>( data.indices.size() ), 1, data.range.getFirst(), baseVertex, 0 },
						static_cast<uint32_t>( uid ) }
				);
				baseVertex += data.vertexCount;
			}

			return records;
		}

		Index size( const Desc::Handle p_handle ) const
		{
			assert( _resources.contains( p_handle ) );

			const auto it = _resources.find( p_handle );
			assert( it != _resources.end() );
			return it->second.range.getCount();
		}

		Index offset( const Desc::Handle p_handle ) const
		{
			assert( _resources.contains( p_handle ) );

			const auto it = _resources.find( p_handle );
			assert( it != _resources.end() );
			return it->second.range.getFirst();
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

		void _removeRange( const Desc::Handle p_handle ) { _resources.erase( p_handle ); }

		/**
		 * @brief Access indices.
		 */
		std::vector<Indice> & _indices( const Desc::Handle p_handle )
		{
			assert( _resources.contains( p_handle ) );

			return _resources[ p_handle ].indices;
		}

		/**
		 * @brief Access per-system geometry resources from specialized geometries.
		 */
		const std::map<Desc::Handle, Data> & _data() const { return _resources; }

	  private:
		/**
		 * @brief Resources.
		 */
		std::map<Desc::Handle, Data> _resources;

		/**
		 * @brief Current size to draw (before applying anything).
		 */
		Index _size = 0;
	};
} // namespace VTX::Renderer::Geometry

#endif
