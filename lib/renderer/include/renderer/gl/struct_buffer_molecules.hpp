#ifndef __VTX_RENDERER_GL_STRUCT_BUFFER_MOLECULES__
#define __VTX_RENDERER_GL_STRUCT_BUFFER_MOLECULES__

#include "buffer.hpp"
#include "vertex_array.hpp"

namespace VTX::Renderer::GL
{
	struct StructBufferMolecules
	{
		Buffer		vboPositions	= Buffer();
		Buffer		vboColors		= Buffer();
		Buffer		vboRadii		= Buffer();
		Buffer		vboVisibilities = Buffer();
		Buffer		vboSelections	= Buffer();
		Buffer		vboIds			= Buffer();
		Buffer		eboBonds		= Buffer();
		VertexArray vao				= VertexArray();
		size_t		sizeAtoms		= 0;
		size_t		sizeBonds		= 0;

		StructBufferMolecules()
		{
			vao.bindElementBuffer( eboBonds );

			// Position.
			vao.enableAttribute( 0 );
			vao.setVertexBuffer<float>( 0, vboPositions, sizeof( Vec3f ) );
			vao.setAttributeFormat<float>( 0, 3 );
			vao.setAttributeBinding( 0, 0 );

			// Color.
			vao.enableAttribute( 1 );
			vao.setVertexBuffer<float>( 1, vboColors, sizeof( Util::Color::Rgba ) );
			vao.setAttributeFormat<float>( 1, 4 );
			vao.setAttributeBinding( 1, 1 );

			// Radius.
			vao.enableAttribute( 2 );
			vao.setVertexBuffer<float>( 2, vboRadii, sizeof( float ) );
			vao.setAttributeFormat<float>( 2, 1 );
			vao.setAttributeBinding( 2, 2 );

			// Visibility.
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
