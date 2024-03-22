#ifndef __VTX_RENDERER_CONTEXT_GL_ENUM_SHADER_TYPE__
#define __VTX_RENDERER_CONTEXT_GL_ENUM_SHADER_TYPE__

#include "renderer/context/include_opengl.hpp"

namespace VTX::Renderer::Context::GL
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
} // namespace VTX::Renderer::Context::GL

#endif
