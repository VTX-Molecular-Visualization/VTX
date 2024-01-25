#ifndef __VTX_RENDERER_CONTEXT_GL_FRAMEBUFFER__
#define __VTX_RENDERER_CONTEXT_GL_FRAMEBUFFER__

#include "texture_2d.hpp"
#include <cassert>
#include <glad/glad.h>
#include <vector>

namespace VTX::Renderer::Context::GL
{
	class Framebuffer
	{
	  public:
		Framebuffer()
		{
			assert( _id == GL_INVALID_INDEX );

			glCreateFramebuffers( 1, &_id );

			assert( glIsFramebuffer( _id ) );
		}

		~Framebuffer()
		{
			assert( _target == 0 );

			if ( _id != GL_INVALID_INDEX )
			{
				glDeleteFramebuffers( 1, &_id );
				_id = GL_INVALID_INDEX;
			}
		}

		inline GLuint getId() const { return _id; }

		inline void bind( const GLenum p_target = GL_FRAMEBUFFER )
		{
			assert( glIsFramebuffer( _id ) );
			assert( glCheckFramebufferStatus( GL_FRAMEBUFFER ) == GL_FRAMEBUFFER_COMPLETE );
			assert( _target == 0 );
			assert( p_target != 0 );

			_target = p_target;
			glBindFramebuffer( _target, _id );
		}

		inline static void bindDefault( const GLuint p_id, const GLenum p_target = GL_FRAMEBUFFER )
		{
			assert( p_target != 0 );

			glBindFramebuffer( p_target, p_id );
		}

		inline void unbind()
		{
			assert( _target != 0 );

			glBindFramebuffer( _target, 0 );
			_target = 0;
		}

		inline static void unbindDefault( const GLenum p_target = GL_FRAMEBUFFER )
		{
			assert( p_target != 0 );

			glBindFramebuffer( p_target, 0 );
		}

		inline void attachTexture( const Texture2D & p_texture, const GLenum p_attachment, const GLint p_level = 0 )
		{
			assert( glIsFramebuffer( _id ) );
			assert( glIsTexture( p_texture.getId() ) );

			glNamedFramebufferTexture( _id, p_attachment, p_texture.getId(), p_level );
		}

		inline void setDrawBuffers( const std::vector<GLenum> & p_drawBuffers )
		{
			assert( glIsFramebuffer( _id ) );

			glNamedFramebufferDrawBuffers(
				_id, GLsizei( p_drawBuffers.size() ), static_cast<const GLenum *>( p_drawBuffers.data() )
			);
		}

		inline void setReadBuffer( const GLenum p_readBuffer )
		{
			assert( glIsFramebuffer( _id ) );

			glNamedFramebufferReadBuffer( _id, p_readBuffer );
		}

	  private:
		GLuint _id	   = GL_INVALID_INDEX;
		GLenum _target = 0;
	};
} // namespace VTX::Renderer::Context::GL

#endif // __VTX_GL_FRAMEBUFFER__
