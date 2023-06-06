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
		Buffer		vboRadius		= Buffer();
		Buffer		vboVisibilities = Buffer();
		Buffer		vboSelections	= Buffer();
		Buffer		vboIds			= Buffer();
		Buffer		eboBonds		= Buffer();
		VertexArray vao				= VertexArray();
		size_t		size			= 0;

		StructBufferMolecules() {}
	};

} // namespace VTX::Renderer::GL

#endif
