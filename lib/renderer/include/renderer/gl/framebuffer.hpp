#ifndef __VTX_RENDERER_GL_FRAMEBUFFER__
#define __VTX_RENDERER_GL_FRAMEBUFFER__

#include "renderer/gl/include_opengl.hpp"
#include <cassert>

namespace VTX::Renderer::GL
{
	class Framebuffer
	{
	  public:
		Framebuffer() = default;

		~Framebuffer() { destroy(); }

		inline void create()
		{
			assert( _id == GL_INVALID_INDEX );

			glCreateFramebuffers( 1, &_id );

			assert( glIsFramebuffer( _id ) );
		}

		inline void destroy()
		{
			assert( _id != GL_INVALID_INDEX );
			assert( _target == 0 );

			glDeleteFramebuffers( 1, &_id );
			_id = GL_INVALID_INDEX;
		}

		inline GLuint getId() const { return _id; }

		inline void assign( const GLuint p_id )
		{
			assert( glIsFramebuffer( p_id ) );

			_id = p_id;
		}

		inline void clear( const GLbitfield p_clear ) const { glClear( p_clear ); }

		inline void bind( const GLenum p_target )
		{
			assert( glIsFramebuffer( _id ) );
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

		inline void attachTexture( const Texture2D & p_texture,
								   const GLenum		 p_attachment,
								   const GLint		 p_level = 0 ) const
		{
			assert( glIsFramebuffer( _id ) );
			assert( glIsTexture( p_texture.getId() ) );

			glNamedFramebufferTexture( _id, p_attachment, p_texture.getId(), p_level );
		}

		inline void setDrawBuffers( const std::vector<GLenum> & p_drawBuffers ) const
		{
			assert( glIsFramebuffer( _id ) );

			glNamedFramebufferDrawBuffers(
				_id, GLsizei( p_drawBuffers.size() ), (const GLenum *)( p_drawBuffers.data() ) );
		}

		inline void setReadBuffer( const GLenum p_readBuffer ) const
		{
			assert( glIsFramebuffer( _id ) );

			glNamedFramebufferReadBuffer( _id, p_readBuffer );
		}

	  private:
		GLuint _id	   = GL_INVALID_INDEX;
		GLenum _target = 0;
	};
} // namespace VTX::Renderer::GL

#endif // __VTX_GL_FRAMEBUFFER__
