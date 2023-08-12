#ifndef __VTX_RENDERER_CONTEXT_OPENGL_45__
#define __VTX_RENDERER_CONTEXT_OPENGL_45__

#include "concept_context.hpp"
#include "renderer/gl/program_manager.hpp"
#include <glad/glad.h>
#include <util/exceptions.hpp>

namespace VTX::Renderer::Context
{
	class OpenGL45 : public BaseContext
	{
	  public:
		OpenGL45() = delete;
		OpenGL45( const size_t	   p_width,
				  const size_t	   p_height,
				  const FilePath & p_shaderPath,
				  void *		   p_proc = nullptr ) :
			BaseContext { p_width, p_height, p_shaderPath }
		{
			if ( p_proc && gladLoadGLLoader( (GLADloadproc)p_proc ) == 0 )
			{
				throw GLException( "Failed to load OpenGL with process" );
			}
			if ( gladLoadGL() == 0 )
			{
				throw GLException( "Failed to load OpenGL" );
			}
			if ( GLAD_GL_VERSION_4_5 == false )
			{
				throw GLException( "OpenGL 4.5 or higher is required" );
			}

			_programManager = std::make_unique<GL::ProgramManager>( p_shaderPath );
		}

		// I/O.
		inline void create( const DescAttachment & p_desc, Handle & p_handle )
		{
			// TODO: m>ap enums.
			// 			glCreateTextures( GL_TEXTURE_2D, 1, &p_handle );
			// 			assert( glIsTexture( p_handle ) );
			// 			glTextureParameteri( p_handle, GL_TEXTURE_WRAP_S, GLint( p_desc.wrappingS ) );
			// 			glTextureParameteri( p_handle, GL_TEXTURE_WRAP_T, GLint( p_desc.wrappingT ) );
			// 			glTextureParameteri( p_handle, GL_TEXTURE_MIN_FILTER, GLint( p_desc.filteringMin ) );
			// 			glTextureParameteri( p_handle, GL_TEXTURE_MAG_FILTER, GLint( p_desc.filteringMag ) );
			// 			glTextureStorage2D( p_handle, 1, GLenum( p_desc.format ), GLsizei( width ), GLsizei( height ) );
		}

		inline void create( const DescStorage & p_desc, Handle & p_handle ) {}

		// Program.
		inline void create( const DescProgram & p_desc, Handle & p_handle )
		{
			_programManager->createProgram( p_desc.name, p_desc.shaders, p_desc.toInject, p_desc.suffix );
		}

	  private:
		enum struct _E_FORMAT : GLenum
		{
			RGBA16F			   = GL_RGBA16F,
			RGBA32UI		   = GL_RGBA32UI,
			RGBA32F			   = GL_RGBA32F,
			RG32UI			   = GL_RG32UI,
			R16F			   = GL_R16F,
			R32F			   = GL_R32F,
			DEPTH_COMPONENT32F = GL_DEPTH_COMPONENT32F
		};

		enum struct _E_WRAPPING : GLint
		{
			REPEAT				 = GL_REPEAT,
			MIRRORED_REPEAT		 = GL_MIRRORED_REPEAT,
			CLAMP_TO_EDGE		 = GL_CLAMP_TO_EDGE,
			CLAMP_TO_BORDER		 = GL_CLAMP_TO_BORDER,
			MIRROR_CLAMP_TO_EDGE = GL_MIRROR_CLAMP_TO_EDGE,
		};

		enum struct _E_FILTERING : GLint
		{
			NEAREST				   = GL_NEAREST,
			LINEAR				   = GL_LINEAR,
			NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
			LINEAR_MIPMAP_NEAREST  = GL_LINEAR_MIPMAP_NEAREST,
			NEAREST_MIPMAP_LINEAR  = GL_NEAREST_MIPMAP_LINEAR,
			LINEAR_MIPMAP_LINEAR   = GL_LINEAR_MIPMAP_LINEAR,
		};

		std::unique_ptr<GL::ProgramManager> _programManager;
	};
} // namespace VTX::Renderer::Context

#endif
