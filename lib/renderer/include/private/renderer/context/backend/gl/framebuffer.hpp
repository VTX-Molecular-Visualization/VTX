#ifndef __VTX_RENDERER_CONTEXT_GL_FRAMEBUFFER__
#define __VTX_RENDERER_CONTEXT_GL_FRAMEBUFFER__

#include "include_opengl.hpp"
#include "texture.hpp"
#include <cassert>
#include <utility>
#include <vector>

namespace VTX::Renderer::Context::Backend::GL
{
	class Framebuffer
	{
	  public:
		Framebuffer() noexcept
		{
			assert( _id == GL_INVALID_INDEX );

			glCreateFramebuffers( 1, &_id );

			assert( glIsFramebuffer( _id ) );
		}

		Framebuffer( const GLuint p_id ) noexcept
		{
			assert( _id == GL_INVALID_INDEX );

			_id = p_id;
		}

		Framebuffer( const Framebuffer & )			   = delete;
		Framebuffer & operator=( const Framebuffer & ) = delete;

		Framebuffer( Framebuffer && p_other ) noexcept : _id( std::exchange( p_other._id, GL_INVALID_INDEX ) ) {}

		Framebuffer & operator=( Framebuffer && p_other ) noexcept
		{
			if ( this != &p_other )
			{
				destroy();
				_id = std::exchange( p_other._id, GL_INVALID_INDEX );
			}

			return *this;
		}

		~Framebuffer() noexcept { destroy(); }

		void destroy() noexcept
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
#ifdef _DEBUG
			if ( _id != 0 )
			{
				assert( glIsFramebuffer( _id ) );
			}
#endif
			glBindFramebuffer( p_target, _id );
		}

		inline void attachTexture(
			const Texture & p_texture,
			const GLenum	p_attachment,
			const GLint		p_level = 0
		) const noexcept
		{
			assert( glIsFramebuffer( _id ) );
			assert( glIsTexture( static_cast<GLuint>( p_texture.getId() ) ) );
			assert( p_texture.getTarget() == GL_TEXTURE_2D );

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

		{ return glCheckNamedFramebufferStatus( _id, GL_FRAMEBUFFER ) == GL_FRAMEBUFFER_COMPLETE; }

	  private:
		GLuint _id = GL_INVALID_INDEX;
	};
} // namespace VTX::Renderer::Context::Backend::GL

#endif // __VTX_GL_FRAMEBUFFER__
