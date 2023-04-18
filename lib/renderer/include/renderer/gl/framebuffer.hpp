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

		void create( const GLenum & p_target )
		{
			assert( _id == GL_INVALID_INDEX );

			_target = p_target;
			glCreateFramebuffers( 1, &_id );

			assert( glIsFramebuffer( _id ) );
		}
		void destroy() { glDeleteFramebuffers( 1, &_id ); }

		void assign( const GLuint p_id )
		{
			assert( glIsFramebuffer( p_id ) );

			_id = p_id;
		}

		int getId() const { return _id; }

		void clear( const GLbitfield p_clear ) const { glClear( p_clear ); }

		void bind() const
		{
			assert( glIsFramebuffer( _id ) );

			glBindFramebuffer( _target, _id );
		}
		void bind( const GLenum p_target ) const
		{
			assert( glIsFramebuffer( _id ) );

			glBindFramebuffer( p_target, _id );
		}
		void unbind() const { glBindFramebuffer( _target, 0 ); }

		void attachTexture( const Texture2D & p_texture, const GLenum p_attachment, const GLint p_level = 0 ) const
		{
			assert( glIsFramebuffer( _id ) );
			assert( glIsTexture( p_texture.getId() ) );

			glNamedFramebufferTexture( _id, p_attachment, p_texture.getId(), p_level );
		}

		void setDrawBuffers( const std::vector<GLenum> & p_drawBuffers ) const
		{
			assert( glIsFramebuffer( _id ) );

			glNamedFramebufferDrawBuffers(
				_id, GLsizei( p_drawBuffers.size() ), (const GLenum *)( p_drawBuffers.data() ) );
		}

		void setReadBuffer( const GLenum & p_readBuffer ) const
		{
			assert( glIsFramebuffer( _id ) );

			glNamedFramebufferReadBuffer( _id, p_readBuffer );
		}

	  private:
		GLuint _id	   = GL_INVALID_INDEX;
		GLenum _target = GL_DRAW_FRAMEBUFFER;
	};
} // namespace VTX::Renderer::GL

#endif // __VTX_GL_FRAMEBUFFER__
