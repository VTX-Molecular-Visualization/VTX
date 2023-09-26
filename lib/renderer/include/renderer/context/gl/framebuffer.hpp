#ifndef __VTX_RENDERER_CONTEXT_GL_FRAMEBUFFER__
#define __VTX_RENDERER_CONTEXT_GL_FRAMEBUFFER__

#include "texture_2d.hpp"
#include <cassert>
#include <glad/glad.h>
#include <vector>

namespace VTX::Renderer::Context::GL::Framebuffer
{
	inline void create( GLuint * const p_handle )
	{
		assert( glIsFramebuffer( *p_handle ) == false );
		glCreateFramebuffers( 1, p_handle );
		assert( glIsFramebuffer( *p_handle ) );
	}

	inline void destroy( GLuint * const p_handle )
	{
		assert( glIsFramebuffer( *p_handle ) );
		glDeleteFramebuffers( 1, p_handle );
	}

	inline void clear( const GLbitfield p_clear ) { glClear( p_clear ); }

	inline void bind( const GLuint p_handle, const GLenum p_target = GL_FRAMEBUFFER )
	{
		assert( glIsFramebuffer( p_handle ) );
		assert( p_target != 0 );
		assert( glCheckFramebufferStatus( GL_FRAMEBUFFER ) == GL_FRAMEBUFFER_COMPLETE );
		glBindFramebuffer( p_target, p_handle );
	}

	inline void unbind( const GLenum p_target = GL_FRAMEBUFFER )
	{
		assert( p_target != 0 );
		glBindFramebuffer( p_target, 0 );
	}

	inline void attachTexture( const GLuint p_handle,
							   const GLuint p_texture,
							   const GLenum p_attachment,
							   const GLint	p_level = 0 )
	{
		assert( glIsFramebuffer( p_handle ) );
		assert( glIsTexture( p_texture ) );
		glNamedFramebufferTexture( p_handle, p_attachment, p_texture, p_level );
	}

	inline void setDrawBuffers( const GLuint p_handle, const std::vector<GLenum> & p_drawBuffers )
	{
		assert( glIsFramebuffer( p_handle ) );
		glNamedFramebufferDrawBuffers(
			p_handle, GLsizei( p_drawBuffers.size() ), static_cast<const GLenum *>( p_drawBuffers.data() ) );
	}

	inline void setReadBuffer( const GLuint p_handle, const GLenum p_readBuffer )
	{
		assert( glIsFramebuffer( p_handle ) );
		glNamedFramebufferReadBuffer( p_handle, p_readBuffer );
	}

} // namespace VTX::Renderer::Context::GL::Framebuffer

#endif
