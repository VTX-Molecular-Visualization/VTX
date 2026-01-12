#ifndef __VTX_RENDERER_CONTEXT_GL_FRAMEBUFFER__
#define __VTX_RENDERER_CONTEXT_GL_FRAMEBUFFER__

#include "include_opengl.hpp"
#include "texture_2d.hpp"
#include <cassert>
#include <vector>

namespace VTX::Renderer::Context::GL
{
	class Framebuffer
	{
	  public:
		Framebuffer() noexcept
		{
			assert( _id == GL_INVALID_INDEX );

			glCreateFramebuffers( 1, &_id );
			// VTX_ERROR( "Framebuffer created: {}", _id );

			assert( glIsFramebuffer( _id ) );
		}

		~Framebuffer() noexcept
		{
			if ( _id != GL_INVALID_INDEX )
			{
				glDeleteFramebuffers( 1, &_id );
				_id = GL_INVALID_INDEX;
			}
		}

		inline GLuint getId() const noexcept { return _id; }

		inline void bind( const GLenum p_target = GL_FRAMEBUFFER ) const noexcept
		{
			assert( glIsFramebuffer( _id ) );
			glBindFramebuffer( p_target, _id );
		}

		inline static void bindDefault( const GLenum p_target = GL_FRAMEBUFFER ) noexcept
		{
			assert( p_target != 0 );
			glBindFramebuffer( p_target, 0 );
		}

		inline void attachTexture(
			const Texture2D & p_texture,
			const GLenum	  p_attachment,
			const GLint		  p_level = 0
		) const noexcept
		{
			assert( glIsFramebuffer( _id ) );
			assert( glIsTexture( static_cast<GLuint>( p_texture.getId() ) ) );

			glNamedFramebufferTexture( _id, p_attachment, static_cast<GLuint>( p_texture.getId() ), p_level );
		}

		inline void setDrawBuffers( const std::vector<GLenum> & p_drawBuffers ) const noexcept
		{
			assert( glIsFramebuffer( _id ) );

			glNamedFramebufferDrawBuffers( _id, static_cast<GLsizei>( p_drawBuffers.size() ), p_drawBuffers.data() );
		}

		inline void setReadBuffer( const GLenum p_readBuffer ) const noexcept
		{
			assert( glIsFramebuffer( _id ) );

			glNamedFramebufferReadBuffer( _id, p_readBuffer );
		}

		inline bool checkStatus() const noexcept

		{
			return glCheckNamedFramebufferStatus( _id, GL_FRAMEBUFFER ) == GL_FRAMEBUFFER_COMPLETE;
		}

	  private:
		GLuint _id = GL_INVALID_INDEX;
	};
} // namespace VTX::Renderer::Context::GL

#endif // __VTX_GL_FRAMEBUFFER__
