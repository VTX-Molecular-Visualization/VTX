#ifndef __VTX_RENDERER_CONTEXT_GL_TEXTURE_2D__
#define __VTX_RENDERER_CONTEXT_GL_TEXTURE_2D__

#include <cassert>
#include <glad/glad.h>

namespace VTX::Renderer::Context::GL::Texture2D
{
	inline void create( GLuint * const p_handle,
						const GLsizei  p_width,
						const GLsizei  p_height,
						const GLenum   p_format,
						const GLenum   p_wrappingS,
						const GLenum   p_wrappingT,
						const GLint	   p_minFilter,
						const GLint	   p_magFilter )
	{
		assert( p_width > 0 && p_height > 0 );
		assert( glIsTexture( *p_handle ) == false );
		glCreateTextures( GL_TEXTURE_2D, 1, p_handle );
		assert( glIsTexture( *p_handle ) );
		glTextureParameteri( *p_handle, GL_TEXTURE_WRAP_S, p_wrappingS );
		glTextureParameteri( *p_handle, GL_TEXTURE_WRAP_T, p_wrappingT );
		glTextureParameteri( *p_handle, GL_TEXTURE_MIN_FILTER, p_minFilter );
		glTextureParameteri( *p_handle, GL_TEXTURE_MAG_FILTER, p_magFilter );
		glTextureStorage2D( *p_handle, 1, p_format, p_width, p_height );
	}

	inline void destroy( GLuint * const p_handle )
	{
		assert( glIsTexture( *p_handle ) );
		glDeleteTextures( 1, p_handle );
	}

	inline void bind( const GLuint p_handle, const GLenum p_target )
	{
		assert( glIsBuffer( p_handle ) );
		assert( p_target != 0 );
		glBindTexture( p_target, p_handle );
	}

	inline void unbind( const GLenum p_target ) { glBindTexture( p_target, 0 ); }

	inline void bindToUnit( const GLuint p_handle, const GLuint p_index )
	{
		assert( glIsBuffer( p_handle ) );
		glBindTextureUnit( p_index, p_handle );
	}

	inline void unbindFromUnit( const GLuint p_index ) { glBindTextureUnit( p_index, 0 ); }

	inline void fill( const GLuint	p_handle,
					  const void *	p_pixels,
					  const GLsizei p_width,
					  const GLsizei p_height,
					  const GLenum	p_format  = GL_RGB,
					  const GLenum	p_type	  = GL_FLOAT,
					  const GLint	p_level	  = 0,
					  const GLint	p_offsetX = 0,
					  const GLint	p_offsetY = 0 )
	{
		assert( glIsBuffer( p_handle ) );
		glTextureSubImage2D( p_handle, p_level, p_offsetX, p_offsetY, p_width, p_height, p_format, p_type, p_pixels );
	}

} // namespace VTX::Renderer::Context::GL::Texture2D

#endif
