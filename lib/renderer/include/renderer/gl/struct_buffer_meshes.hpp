#ifndef __VTX_RENDERER_GL_STRUCT_BUFFER_MESHES__
#define __VTX_RENDERER_GL_STRUCT_BUFFER_MESHES__

#include "buffer.hpp"
#include "vertex_array.hpp"
#include <util/color/rgba.hpp>

namespace VTX::Renderer::GL
{
	struct StructBufferMeshes
	{
		Buffer		vboPositions	= Buffer();
		Buffer		vboNormals		= Buffer();
		Buffer		vboColors		= Buffer();
		Buffer		vboVisibilities = Buffer();
		Buffer		vboSelections	= Buffer();
		Buffer		vboIds			= Buffer();
		Buffer		ebo				= Buffer();
		VertexArray vao				= VertexArray();
		size_t		size			= 0;

		StructBufferMeshes()
		{
			vboPositions.create();
			vboNormals.create();
			vboColors.create();
			vboVisibilities.create();
			vboSelections.create();
			vboIds.create();
			ebo.create();

			vao.create();

			vao.bindElementBuffer( ebo );

			// Position.
			vao.enableAttribute( 0 );
			vao.setVertexBuffer<float>( 0, vboPositions, sizeof( Vec3f ) );
			vao.setAttributeFormat<float>( 0, 3 );
			vao.setAttributeBinding( 0, 0 );

			// Normal.
			vao.enableAttribute( 1 );
			vao.setVertexBuffer<float>( 1, vboNormals, sizeof( Vec3f ) );
			vao.setAttributeFormat<float>( 1, 3 );
			vao.setAttributeBinding( 1, 1 );

			// Color.
			vao.enableAttribute( 2 );
			vao.setVertexBuffer<float>( 2, vboColors, sizeof( Util::Color::Rgba ) );
			vao.setAttributeFormat<float>( 2, 4 );
			vao.setAttributeBinding( 2, 2 );

			// Visbility.
			vao.enableAttribute( 3 );
			vao.setVertexBuffer<uint>( 3, vboVisibilities, sizeof( uint ) );
			vao.setAttributeFormat<uint>( 3, 1 );
			vao.setAttributeBinding( 3, 3 );

			// Selection.
			vao.enableAttribute( 4 );
			vao.setVertexBuffer<uint>( 4, vboSelections, sizeof( uint ) );
			vao.setAttributeFormat<uint>( 4, 1 );
			vao.setAttributeBinding( 4, 4 );

			// Id.
			vao.enableAttribute( 5 );
			vao.setVertexBuffer<uint>( 5, vboIds, sizeof( uint ) );
			vao.setAttributeFormat<uint>( 5, 1 );
			vao.setAttributeBinding( 5, 5 );
		}
	};

} // namespace VTX::Renderer::GL

#endif
