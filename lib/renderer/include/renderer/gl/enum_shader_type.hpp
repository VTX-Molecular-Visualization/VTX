#ifndef __VTX_RENDERER_GL_ENUM_SHADER_TYPE__
#define __VTX_RENDERER_GL_ENUM_SHADER_TYPE__

#include "renderer/gl/include_opengl.hpp"

namespace VTX::Renderer::GL
{
	enum class ENUM_SHADER_TYPE : GLuint
	{
		VERTEX			= GL_VERTEX_SHADER,
		FRAGMENT		= GL_FRAGMENT_SHADER,
		GEOMETRY		= GL_GEOMETRY_SHADER,
		COMPUTE			= GL_COMPUTE_SHADER,
		TESS_EVALUATION = GL_TESS_EVALUATION_SHADER,
		TESS_CONTROL	= GL_TESS_CONTROL_SHADER
	};
}

#endif