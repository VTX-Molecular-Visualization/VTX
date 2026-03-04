#ifndef __VTX_RENDERER_GEOMETRIES__
#define __VTX_RENDERER_GEOMETRIES__

#include "renderer/binary_buffer.hpp"
#include "renderer/context/context_wrapper.hpp"
#include "renderer/geometry/cylinder.hpp"
#include "renderer/geometry/ribbon.hpp"
#include "renderer/geometry/ses.hpp"
#include "renderer/geometry/sphere.hpp"
#include "renderer/geometry/voxel.hpp"

namespace VTX::Renderer
{
	struct Geometries
	{
		Geometry::Sphere   spheres;
		Geometry::Cylinder cylinders;
		Geometry::Ribbon   ribbons;
		Geometry::Voxel	   voxels;
		//  Geometry::SES	   ses;

		void construct( const SystemData & p_data )
		{
			spheres.construct( p_data );
			cylinders.construct( p_data );
			ribbons.construct( p_data );
		}

		void buildDrawRanges( Context::ContextWrapper & p_context )
		{
			p_context.setPipelineBuffer( "SphereIndirect", push( spheres.toDrawIndirectCommands() ) );
			p_context.setPipelineBuffer( "CylinderIndirect", push( cylinders.toDrawIndexedIndirectCommands() ) );
			p_context.setPipelineBuffer( "RibbonIndirect", push( ribbons.toDrawIndirectCommands() ) );
		}

		[[nodiscard]] BinaryBuffer430 push( const std::vector<Desc::DrawIndirectCommand> & p_draw )
		{
			BinaryBuffer430 buffer;

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

		[[nodiscard]] BinaryBuffer430 push( const std::vector<Desc::DrawIndexedIndirectCommand> & p_draw )
		{
			BinaryBuffer430 buffer;

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
