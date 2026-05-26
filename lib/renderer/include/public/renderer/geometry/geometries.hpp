#ifndef __VTX_RENDERER_GEOMETRIES__
#define __VTX_RENDERER_GEOMETRIES__

#include "renderer/binary_buffer.hpp"
#include "renderer/context/context_wrapper.hpp"
#include "renderer/descriptors.hpp"
#include "renderer/geometry/cylinder.hpp"
#include "renderer/geometry/grid.hpp"
#include "renderer/geometry/ribbon.hpp"
#include "renderer/geometry/ses.hpp"
#include "renderer/geometry/sphere.hpp"
#include <util/logger.hpp>

namespace VTX::Renderer
{
	class Geometries
	{
	  public:
		Geometry::Sphere   spheres;
		Geometry::Cylinder cylinders;
		Geometry::Ribbon   ribbons;
		Geometry::Grid	   grid;
		Geometry::SES	   ses;

		void clear()
		{
			clearSystems();
			grid.clear();
		}

		void clearSystems()
		{
			spheres.clear();
			cylinders.clear();
			ribbons.clear();
			ses.clear();
		}

		void clearSystemRanges()
		{
			spheres.clearRanges();
			cylinders.clearRanges();
			ribbons.clearRanges();
		}

		void removeSystemConstruction( const Desc::Handle p_handle )
		{
			cylinders.removeConstruction( p_handle );
			ribbons.removeConstruction( p_handle );
			ses.invalidate( p_handle );
		}

		void registerSystem( const Desc::Handle p_handle, const Cache::System & p_data )
		{
			spheres.registerSystem( p_handle, p_data );
			cylinders.registerSystem( p_handle, p_data );
		}

		void constructSES(
			Context::ContextWrapper & p_context,
			const Desc::Handle		  p_handle,
			const Cache::System &	  p_data,
			const uint32_t			  p_inputAtomOffset,
			const float				  p_probeRadius,
			const RepresentationIndex p_representation
		)
		{
			ses.construct( p_context, p_handle, p_data, p_inputAtomOffset, p_probeRadius, p_representation );
		}

		void uploadIndexes( Context::ContextWrapper & p_context, const Desc::Handle p_handle )
		{
			spheres.uploadIndexes( p_context, p_handle );
			cylinders.uploadIndexes( p_context, p_handle );
			if ( ribbons.built( p_handle ) )
			{
				ribbons.uploadIndexes( p_context, p_handle );
			}
			ses.uploadIndexes( p_context, p_handle );
		}

		void resize( Context::ContextWrapper & p_context ) { resizeSystems( p_context ); }

		void resizeSystems( Context::ContextWrapper & p_context )
		{
			spheres.resize( p_context );
			cylinders.resize( p_context );
			ribbons.resize( p_context );
			ses.resize( p_context );
		}

		template<typename Systems>
		void buildDrawRanges( Context::ContextWrapper & p_context, Systems & p_systems )
		{
			auto sphereDraws = spheres.toDrawIndexedIndirectCommands();
			_setModelIndices( sphereDraws, p_systems );
			_logDrawRanges( { Geometry::Sphere::INDIRECT_SPHERES }, sphereDraws );
			p_context.setBuffer( { Geometry::Sphere::INDIRECT_SPHERES }, _toBuffer( sphereDraws ) );

			auto cylinderDraws = cylinders.toDrawIndexedIndirectCommands();
			_setModelIndices( cylinderDraws, p_systems );
			_logDrawRanges( { Geometry::Cylinder::INDIRECT_CYLINDERS }, cylinderDraws );
			p_context.setBuffer( { Geometry::Cylinder::INDIRECT_CYLINDERS }, _toBuffer( cylinderDraws ) );

			auto ribbonDraws = ribbons.toDrawIndexedIndirectCommands();
			_setModelIndices( ribbonDraws, p_systems );
			_logDrawRanges( { Geometry::Ribbon::INDIRECT_RIBBONS }, ribbonDraws );
			p_context.setBuffer( { Geometry::Ribbon::INDIRECT_RIBBONS }, _toBuffer( ribbonDraws ) );

			const auto gridDraws = grid.toDrawIndirectCommands();
			_logDrawRanges( { Geometry::Grid::INDIRECT_GRID }, gridDraws );
			p_context.setBuffer( { Geometry::Grid::INDIRECT_GRID }, _toBuffer( gridDraws ) );
			_uploadPatchDrawCommands(
				p_context, p_systems, { Geometry::SES::INDIRECT_CONVEX_PATCHES }, ses.convexPatches
			);
			_uploadPatchDrawCommands(
				p_context, p_systems, { Geometry::SES::INDIRECT_CIRCLE_PATCHES }, ses.circlePatches
			);
			_uploadPatchDrawCommands(
				p_context, p_systems, { Geometry::SES::INDIRECT_SEGMENT_PATCHES }, ses.segmentPatches
			);
			_uploadPatchDrawCommands(
				p_context, p_systems, { Geometry::SES::INDIRECT_CONCAVE_PATCHES }, ses.concavePatches
			);
		}

	  private:
		template<typename Records, typename Systems>
		void _setModelIndices( Records & p_records, Systems & p_systems )
		{
			for ( auto & record : p_records )
			{
				const Desc::Handle system = record.idModel;
				assert( p_systems.contains( system ) );
				record.idModel = p_systems.get( system ).modelIndex;
			}
		}

		template<typename Systems>
		void _uploadPatchDrawCommands(
			Context::ContextWrapper &			 p_context,
			Systems &							 p_systems,
			const Desc::Key &					 p_indirectBuffer,
			const Geometry::SES::PatchGeometry & p_geometry
		)
		{
			if ( p_geometry.chunks.empty() )
			{
				const std::vector<Desc::DrawIndexedIndirectRecord> records;
				_logDrawRanges( { p_indirectBuffer, uint32_t( 0 ) }, records );
				p_context.setBuffer( Desc::BufferRef { p_indirectBuffer, uint32_t( 0 ) }, _toBuffer( records ) );
				return;
			}

			for ( const Desc::BufferChunk chunk : p_geometry.chunks )
			{
				const Desc::BufferRef ref { p_indirectBuffer, chunk };
				auto				  records = p_geometry.toDrawIndexedIndirectCommands( chunk );
				_setModelIndices( records, p_systems );
				_logDrawRanges( ref, records );
				p_context.ensureBufferChunk( ref );
				p_context.setBuffer( ref, _toBuffer( records ) );
			}
		}

		void _logDrawRanges( const Desc::BufferRef & p_ref, const std::vector<Desc::DrawIndirectRecord> & p_records )
		{
			if ( p_ref.chunk )
			{
				VTX_DEBUG( "[DRAW_RANGES] {} chunk={} count={}", p_ref.key, *p_ref.chunk, p_records.size() );
			}
			else
			{
				VTX_DEBUG( "[DRAW_RANGES] {} count={}", p_ref.key, p_records.size() );
			}

			for ( size_t i = 0; i < p_records.size(); ++i )
			{
				const Desc::DrawIndirectRecord & record = p_records[ i ];
				VTX_TRACE(
					"[DRAW_RANGES] {}[{}] vertexCount={} instanceCount={} firstVertex={} baseInstance={} idModel={}",
					p_ref.key,
					i,
					record.command.vertexCount,
					record.command.instanceCount,
					record.command.firstVertex,
					record.command.baseInstance,
					record.idModel
				);
			}
		}

		void _logDrawRanges(
			const Desc::BufferRef &								 p_ref,
			const std::vector<Desc::DrawIndexedIndirectRecord> & p_records
		)
		{
			if ( p_ref.chunk )
			{
				VTX_DEBUG( "[DRAW_RANGES] {} chunk={} count={}", p_ref.key, *p_ref.chunk, p_records.size() );
			}
			else
			{
				VTX_DEBUG( "[DRAW_RANGES] {} count={}", p_ref.key, p_records.size() );
			}

			for ( size_t i = 0; i < p_records.size(); ++i )
			{
				const Desc::DrawIndexedIndirectRecord & record = p_records[ i ];
				VTX_TRACE(
					"[DRAW_RANGES] {}[{}] indexCount={} instanceCount={} firstIndex={} baseVertex={} "
					"baseInstance={} idModel={} data0={}",
					p_ref.key,
					i,
					record.command.indexCount,
					record.command.instanceCount,
					record.command.firstIndex,
					record.command.baseVertex,
					record.command.baseInstance,
					record.idModel,
					record.padding0
				);
			}
		}

		[[nodiscard]] BinaryBuffer430 _toBuffer( const std::vector<Desc::DrawIndirectRecord> & p_records )
		{
			BinaryBuffer430 buffer;

			buffer.write( static_cast<uint32_t>( p_records.size() ) );
			buffer.alignTo( Desc::DRAW_INDIRECT_COMMANDS_OFFSET );

			for ( const Desc::DrawIndirectRecord & record : p_records )
			{
				buffer.write( record.command.vertexCount );
				buffer.write( record.command.instanceCount );
				buffer.write( record.command.firstVertex );
				buffer.write( record.command.baseInstance );
				buffer.write( record.idModel );
				buffer.write( record.padding0 );
				buffer.write( record.padding1 );
				buffer.write( record.padding2 );
			}
			buffer.close();

			return buffer;
		}

		[[nodiscard]] BinaryBuffer430 _toBuffer( const std::vector<Desc::DrawIndexedIndirectRecord> & p_records )
		{
			BinaryBuffer430 buffer;

			buffer.write( static_cast<uint32_t>( p_records.size() ) );
			buffer.alignTo( Desc::DRAW_INDIRECT_COMMANDS_OFFSET );

			for ( const Desc::DrawIndexedIndirectRecord & record : p_records )
			{
				buffer.write( record.command.indexCount );
				buffer.write( record.command.instanceCount );
				buffer.write( record.command.firstIndex );
				buffer.write( record.command.baseVertex );
				buffer.write( record.command.baseInstance );
				buffer.write( record.idModel );
				buffer.write( record.padding0 );
				buffer.write( record.padding1 );
			}
			buffer.close();

			return buffer;
		}
	};
} // namespace VTX::Renderer

#endif
