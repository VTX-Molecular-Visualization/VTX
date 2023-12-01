#ifndef __VTX_RENDERER_GL_FRAMEBUFFER__
#define __VTX_RENDERER_GL_FRAMEBUFFER__

#include "renderer/gl/include_opengl.hpp"
#include "renderer/gl/texture_2d.hpp"
#include <cassert>
#include <vector>

namespace VTX::Renderer::GL
{
	class Framebuffer
	{
	  public:
		Framebuffer()
		{
			assert( _id == GL_INVALID_INDEX );

#if ( VTX_OPENGL_VERSION == 450 )
			glCreateFramebuffers( 1, &_id );
#else
			glGenFramebuffers( 1, &_id );
#endif

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

		inline void assign( const GLuint p_id )
		{
			assert( glIsFramebuffer( p_id ) );

			_id = p_id;
		}

		inline void clear( const GLbitfield p_clear ) const { glClear( p_clear ); }

		inline void bind( const GLenum p_target = GL_FRAMEBUFFER )
		{
			assert( glIsFramebuffer( _id ) );
			assert( glCheckFramebufferStatus( GL_FRAMEBUFFER ) == GL_FRAMEBUFFER_COMPLETE );
			assert( _target == 0 );
			assert( p_target != 0 );

			_target = p_target;
			glBindFramebuffer( _target, _id );
		}

		inline void unbind()
		{
			assert( _target != 0 );

			glBindFramebuffer( _target, 0 );
			_target = 0;
		}

		inline void attachTexture( const Texture2D & p_texture, const GLenum p_attachment, const GLint p_level = 0 )
		{
			assert( glIsFramebuffer( _id ) );
			assert( glIsTexture( p_texture.getId() ) );

#if ( VTX_OPENGL_VERSION == 450 )
			glNamedFramebufferTexture( _id, p_attachment, p_texture.getId(), p_level );
#else
			bind();
			glFramebufferTexture( _target, p_attachment, p_texture.getId(), p_level );
			unbind();
#endif
		}

		inline void setDrawBuffers( const std::vector<GLenum> & p_drawBuffers )
		{
			assert( glIsFramebuffer( _id ) );

#if ( VTX_OPENGL_VERSION == 450 )
			glNamedFramebufferDrawBuffers(
				_id, GLsizei( p_drawBuffers.size() ), static_cast<const GLenum *>( p_drawBuffers.data() ) );
#else
			bind();
			glDrawBuffers( GLsizei( p_drawBuffers.size() ), static_cast<const GLenum *>( p_drawBuffers.data() ) );
			unbind();
#endif
		}

		inline void setReadBuffer( const GLenum p_readBuffer )
		{
			assert( glIsFramebuffer( _id ) );

#if ( VTX_OPENGL_VERSION == 450 )
			glNamedFramebufferReadBuffer( _id, p_readBuffer );
#else
			bind();
			glReadBuffer( p_readBuffer );
			unbind();
#endif
		}

	  private:
		GLuint _id	   = GL_INVALID_INDEX;
		GLenum _target = 0;
	};
} // namespace VTX::Renderer::GL

#endif // __VTX_GL_FRAMEBUFFER__
