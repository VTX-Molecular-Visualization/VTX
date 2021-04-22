#ifndef __VTX_GL_FRAMEBUFFER__
#define __VTX_GL_FRAMEBUFFER__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include "generic/base_opengl.hpp"
#include "texture_2d.hpp"

namespace VTX::Renderer::GL
{
	class Framebuffer : public Generic::BaseOpenGL
	{
	  public:
		enum Attachment
		{
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
		enum Target
		{
			DRAW_FRAMEBUFFER = GL_DRAW_FRAMEBUFFER,
			READ_FRAMEBUFFER = GL_READ_FRAMEBUFFER,
			FRAMEBUFFER		 = GL_FRAMEBUFFER
		};

		Framebuffer( OpenGLFunctions * const p_gl ) : BaseOpenGL( p_gl ) { _gl->glCreateFramebuffers( 1, &_id ); }
		~Framebuffer() { _gl->glDeleteFramebuffers( 1, &_id ); }

		int getId() const { return _id; }

		void bind( const Target p_target = Target::DRAW_FRAMEBUFFER ) { _gl->glBindFramebuffer( p_target, _id ); }

		void attachTexture( const Texture2D & p_texture, const Attachment p_attachment, const int p_level = 0 )
		{
			_gl->glNamedFramebufferTexture( _id, p_attachment, p_texture.getId(), p_level );
		}

	  private:
		GLuint _id = GL_INVALID_INDEX;
	};
} // namespace VTX::Renderer::GL

#endif // __VTX_GL_FRAMEBUFFER__
