#ifndef __VTX_RENDERER_GL_INCLUDE_OPENGL__
#define __VTX_RENDERER_GL_INCLUDE_OPENGL__

#include <glad/glad.h>
#include <util/exceptions.hpp>

// TODO: delete and handle multiple versions with contexts.
#define VTX_OPENGL_VERSION 450

namespace VTX::Renderer::GL
{
	enum class VTX_GL_SUPPORTED_VERSIONS
	{
		GL_4_1,
		GL_4_5
	};

	static VTX_GL_SUPPORTED_VERSIONS OpenGLVersion;

	inline void loadOpenGL( void * p_proc = nullptr )
	{
		if ( p_proc && gladLoadGLLoader( (GLADloadproc)p_proc ) == 0 )
		{
			throw GLException( "Failed to load OpenGL" );
		}
		else if ( gladLoadGL() == 0 )
		{
			throw GLException( "Failed to load OpenGL" );
		}

		// Check OpenGL version.
		if ( GLAD_GL_VERSION_4_5 )
		{
			OpenGLVersion = VTX_GL_SUPPORTED_VERSIONS::GL_4_5;
		}
		else if ( GLAD_GL_VERSION_4_1 )
		{
			OpenGLVersion = VTX_GL_SUPPORTED_VERSIONS::GL_4_1;
		}
		else
		{
			throw GLException( "OpenGL 4.1 or higher is required" );
		}
	}
} // namespace VTX::Renderer::GL

#endif
