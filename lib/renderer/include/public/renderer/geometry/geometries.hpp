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
			ribbons.construct( p_handle, p_data );
			ses.construct( p_handle, p_data );
		}

		void uploadIndexes( Context::ContextWrapper & p_context, const Desc::Handle p_handle )
		{
			spheres.uploadIndexes( p_context, p_handle );
			cylinders.uploadIndexes( p_context, p_handle );
			ribbons.uploadIndexes( p_context, p_handle );
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
				Geometry::Sphere::INDIRECT_SPHERES, _toBuffer( spheres.toDrawIndexedIndirectCommands() )
			);
			p_context.setBuffer(
				Geometry::Cylinder::INDIRECT_CYLINDERS, _toBuffer( cylinders.toDrawIndexedIndirectCommands() )
			);
			p_context.setBuffer(
				Geometry::Ribbon::INDIRECT_RIBBONS, _toBuffer( ribbons.toDrawIndexedIndirectCommands() )
			);
			p_context.setBuffer( Geometry::Grid::INDIRECT_GRID, _toBuffer( grid.toDrawIndirectCommands() ) );
			p_context.setBuffer(
				Geometry::SES::INDIRECT_CONVEX_PATCHES, _toBuffer( ses.convexPatches.toDrawIndexedIndirectCommands() )
			);
			p_context.setBuffer(
				Geometry::SES::INDIRECT_CIRCLE_PATCHES, _toBuffer( ses.circlePatches.toDrawIndexedIndirectCommands() )
			);
			p_context.setBuffer(
				Geometry::SES::INDIRECT_SEGMENT_PATCHES, _toBuffer( ses.segmentPatches.toDrawIndexedIndirectCommands() )
			);
			p_context.setBuffer(
				Geometry::SES::INDIRECT_CONCAVE_PATCHES, _toBuffer( ses.concavePatches.toDrawIndexedIndirectCommands() )
			);
		}

	  private:
		[[nodiscard]] BinaryBuffer430 _toBuffer( const std::vector<Desc::DrawIndirectCommand> & p_draw )
		{
			BinaryBuffer430 buffer;

			buffer.write( static_cast<uint32_t>( p_draw.size() ) );
			buffer.alignTo( Desc::DRAW_INDIRECT_COMMANDS_OFFSET );

			for ( const Desc::DrawIndirectCommand & draw : p_draw )
			{
				buffer.write( draw.vertexCount );
				buffer.write( draw.instanceCout );
				buffer.write( draw.firstVertex );
				buffer.write( draw.baseInstance );
			}
			buffer.close();

			return buffer;
		}

		[[nodiscard]] BinaryBuffer430 _toBuffer( const std::vector<Desc::DrawIndexedIndirectCommand> & p_draw )
		{
			BinaryBuffer430 buffer;

			buffer.write( static_cast<uint32_t>( p_draw.size() ) );
			buffer.alignTo( Desc::DRAW_INDIRECT_COMMANDS_OFFSET );

			for ( const Desc::DrawIndexedIndirectCommand & draw : p_draw )
			{
				buffer.write( draw.indexCount );
				buffer.write( draw.instanceCount );
				buffer.write( draw.firstIndex );
				buffer.write( draw.baseVertex );
				buffer.write( draw.baseInstance );
			}
			buffer.close();

			return buffer;
		}
	};
} // namespace VTX::Renderer

#endif
