#ifndef __VTX_RENDERER_GEOMETRIES__
#define __VTX_RENDERER_GEOMETRIES__

#include "renderer/binary_buffer.hpp"
#include "renderer/context/context_wrapper.hpp"
#include "renderer/geometry/cylinder.hpp"
#include "renderer/geometry/grid.hpp"
#include "renderer/geometry/ribbon.hpp"
#include "renderer/geometry/ses.hpp"
#include "renderer/geometry/sphere.hpp"

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

		void construct( const Desc::Handle p_handle, const SystemData & p_data )
		{
			spheres.construct( p_handle, p_data );
			cylinders.construct( p_handle, p_data );
		}

		void constructSES(
			Context::ContextWrapper & p_context,
			const Desc::Handle		  p_handle,
			const SystemData &		  p_data,
			const uint32_t			  p_inputAtomOffset
		)
		{
			ses.construct( p_context, p_handle, p_data, p_inputAtomOffset );
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

		void buildDrawRanges( Context::ContextWrapper & p_context )
		{
			p_context.setBuffer(
				{ Geometry::Sphere::INDIRECT_SPHERES }, _toBuffer( spheres.toDrawIndexedIndirectCommands() )
			);
			p_context.setBuffer(
				{ Geometry::Cylinder::INDIRECT_CYLINDERS }, _toBuffer( cylinders.toDrawIndexedIndirectCommands() )
			);
			p_context.setBuffer(
				{ Geometry::Ribbon::INDIRECT_RIBBONS }, _toBuffer( ribbons.toDrawIndexedIndirectCommands() )
			);
			p_context.setBuffer( { Geometry::Grid::INDIRECT_GRID }, _toBuffer( grid.toDrawIndirectCommands() ) );
			_uploadPatchDrawCommands( p_context, { Geometry::SES::INDIRECT_CONVEX_PATCHES }, ses.convexPatches );
			_uploadPatchDrawCommands( p_context, { Geometry::SES::INDIRECT_CIRCLE_PATCHES }, ses.circlePatches );
			_uploadPatchDrawCommands( p_context, { Geometry::SES::INDIRECT_SEGMENT_PATCHES }, ses.segmentPatches );
			_uploadPatchDrawCommands( p_context, { Geometry::SES::INDIRECT_CONCAVE_PATCHES }, ses.concavePatches );
		}

	  private:
		void _uploadPatchDrawCommands(
			Context::ContextWrapper &			 p_context,
			const Desc::Key &					 p_indirectBuffer,
			const Geometry::SES::PatchGeometry & p_geometry
		)
		{
			if ( p_geometry.chunks.empty() )
			{
				p_context.setBuffer(
					Desc::BufferRef { p_indirectBuffer, uint32_t( 0 ) },
					_toBuffer( std::vector<Desc::DrawIndexedIndirectRecord> {} )
				);
				return;
			}

			for ( const BufferChunk chunk : p_geometry.chunks )
			{
				const Desc::BufferRef ref { p_indirectBuffer, chunk };
				p_context.ensureBufferChunk( ref );
				p_context.setBuffer( ref, _toBuffer( p_geometry.toDrawIndexedIndirectCommands( chunk ) ) );
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
