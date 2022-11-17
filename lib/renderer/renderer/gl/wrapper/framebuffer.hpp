#ifndef __VTX_GL_FRAMEBUFFER__
#define __VTX_GL_FRAMEBUFFER__

#include "texture_2d.hpp"
#include <GL/gl3w.h>
#include <cassert>
#include <vector>

namespace VTX::Renderer::GL::Wrapper
{
	class Framebuffer
	{
	  public:
		enum class Attachment : GLenum
		{
			// See http://docs.gl/gl4/glFramebufferTexture.
			COLOR0	= GL_COLOR_ATTACHMENT0,
			COLOR1	= GL_COLOR_ATTACHMENT1,
			COLOR2	= GL_COLOR_ATTACHMENT2,
			COLOR3	= GL_COLOR_ATTACHMENT3,
			COLOR4	= GL_COLOR_ATTACHMENT4,
			COLOR5	= GL_COLOR_ATTACHMENT5,
			COLOR6	= GL_COLOR_ATTACHMENT6,
			COLOR7	= GL_COLOR_ATTACHMENT7,
			COLOR8	= GL_COLOR_ATTACHMENT8,
			COLOR9	= GL_COLOR_ATTACHMENT9,
			COLOR10 = GL_COLOR_ATTACHMENT10,
			COLOR11 = GL_COLOR_ATTACHMENT11,
			COLOR12 = GL_COLOR_ATTACHMENT12,
			COLOR13 = GL_COLOR_ATTACHMENT13,
			COLOR14 = GL_COLOR_ATTACHMENT14,
			COLOR15 = GL_COLOR_ATTACHMENT15,
			COLOR16 = GL_COLOR_ATTACHMENT16,
			COLOR17 = GL_COLOR_ATTACHMENT17,
			COLOR18 = GL_COLOR_ATTACHMENT18,
			COLOR19 = GL_COLOR_ATTACHMENT19,
			COLOR20 = GL_COLOR_ATTACHMENT20,
			COLOR21 = GL_COLOR_ATTACHMENT21,
			COLOR22 = GL_COLOR_ATTACHMENT22,
			COLOR23 = GL_COLOR_ATTACHMENT23,
			COLOR24 = GL_COLOR_ATTACHMENT24,
			COLOR25 = GL_COLOR_ATTACHMENT25,
			COLOR26 = GL_COLOR_ATTACHMENT26,
			COLOR27 = GL_COLOR_ATTACHMENT27,
			COLOR28 = GL_COLOR_ATTACHMENT28,
			COLOR29 = GL_COLOR_ATTACHMENT29,
			COLOR30 = GL_COLOR_ATTACHMENT30,
			COLOR31 = GL_COLOR_ATTACHMENT31,
			DEPTH	= GL_DEPTH_ATTACHMENT,
			STENCIL = GL_STENCIL_ATTACHMENT
		};
		enum class Target : GLenum
		{
			// See http://docs.gl/gl4/glFramebufferTexture.
			DRAW_FRAMEBUFFER = GL_DRAW_FRAMEBUFFER,
			READ_FRAMEBUFFER = GL_READ_FRAMEBUFFER,
			FRAMEBUFFER		 = GL_FRAMEBUFFER
		};
		enum class ClearBuffer : GLbitfield
		{
			// See http://docs.gl/gl4/glClear.
			COLOR		= GL_COLOR_BUFFER_BIT,
			DEPTH		= GL_DEPTH_BUFFER_BIT,
			STENCIL		= GL_STENCIL_BUFFER_BIT,
			COLOR_DEPTH = COLOR | DEPTH,
			ALL			= COLOR | DEPTH | STENCIL
		};

		Framebuffer() = default;

		~Framebuffer() { destroy(); }

		void create( const Target & p_target )
		{
			assert( _id == GL_INVALID_INDEX );

			_target = p_target;
			glCreateFramebuffers( 1, &_id );

			assert( glIsFramebuffer( _id ) );
		}
		void destroy()
		{
			if ( _handleDeletion )
				glDeleteFramebuffers( 1, &_id );
		}

		void assign( const GLuint p_id )
		{
			assert( glIsFramebuffer( p_id ) );

			_id				= p_id;
			_handleDeletion = false;
		}

		int getId() const { return _id; }

		void clear( const ClearBuffer p_clear ) const { glClear( GLbitfield( p_clear ) ); }

		void bind() const
		{
			assert( glIsFramebuffer( _id ) );

			glBindFramebuffer( GLenum( _target ), _id );
		}
		void bind( const Target p_target ) const
		{
			assert( glIsFramebuffer( _id ) );

			glBindFramebuffer( GLenum( p_target ), _id );
		}
		void unbind() const { glBindFramebuffer( GLenum( _target ), 0 ); }

		void attachTexture( const Texture2D & p_texture, const Attachment p_attachment, const GLint p_level = 0 ) const
		{
			assert( glIsFramebuffer( _id ) );
			assert( glIsTexture( p_texture.getId() ) );

			glNamedFramebufferTexture( _id, GLenum( p_attachment ), p_texture.getId(), p_level );
		}

		void setDrawBuffers( const std::vector<Attachment> & p_drawBuffers ) const
		{
			assert( glIsFramebuffer( _id ) );

			glNamedFramebufferDrawBuffers(
				_id, GLsizei( p_drawBuffers.size() ), (const GLenum *)( p_drawBuffers.data() ) );

			_checkStatus();
		}

		void setReadBuffer( const Attachment & p_readBuffer ) const
		{
			assert( glIsFramebuffer( _id ) );

			glNamedFramebufferReadBuffer( _id, GLenum( p_readBuffer ) );

			_checkStatus();
		}

	  private:
		void _checkStatus() const;

	  private:
		GLuint _id	   = GL_INVALID_INDEX;
		Target _target = Target::DRAW_FRAMEBUFFER;
		/// TODO: this is a fix to handle Qt framebuffer
		bool _handleDeletion = true;
	};
} // namespace VTX::Renderer::GL::Wrapper

#endif // __VTX_GL_FRAMEBUFFER__
