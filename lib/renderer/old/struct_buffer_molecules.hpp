#ifndef __VTX_RENDERER_GL_STRUCT_BUFFER_MOLECULES__
#define __VTX_RENDERER_GL_STRUCT_BUFFER_MOLECULES__

#include "buffer.hpp"
#include "vertex_array.hpp"

namespace VTX::Renderer::GL
{
	struct StructBufferMolecules
	{
		Buffer vboPositions	   = Buffer();
		Buffer vboColors	   = Buffer();
		Buffer vboRadii		   = Buffer();
		Buffer vboVisibilities = Buffer();
		Buffer vboSelections   = Buffer();
		Buffer vboIds		   = Buffer();
		Buffer eboBonds		   = Buffer();
		// Buffer		eboSelection	= Buffer();
		VertexArray vao		  = VertexArray();
		size_t		sizeAtoms = 0;
		size_t		sizeBonds = 0;

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

			// VAO/VBO/EBO.
			// vec4[atom count] // position, vdWRadius or radius applied from representation?
			// uint[atom count] // model id (to get data from model ubo)
			// uint[atom count] // color id (to get data from color ubo)
			// uint[atom count] // representation id
			// uint[atom count] // atom id (for picking)
			// uint[atom count] // is visible // delete and use multi draw, or update buffer?
			// uint[atom count] // is selected // to delete and add a pass with only selected atoms
			// uint[bond count] // bonds ebo

			// UBO
			// Models: { mat4 mat4 }[molecule count] // model and normal matrix
			// Colors: vec4[256][] // color layouts
			// Representations: { float float bool float uint}[repsentation count] //  sphereRadiusFixed sphereRadiusAdd
			// isSphereRadiusFixed cylinderRadius colorBlendingMode // representation infos

			// Delete some radius infos in rep and compute CPU side (of compute shader) in position.w?

			// Use SSBO and compute data with compute shader if possible?

			// uint[atom count] // symbol id (to get data from symbol infos ubo)
			// Symbol infos: float[symbol count] // symbol infos by id (only radius for the moment)
		}

		void bindBonds() { vao.bindElementBuffer( eboBonds ); }

		// void bindSelection() { vao.bindElementBuffer( eboSelection ); }
	};

} // namespace VTX::Renderer::GL

#endif
