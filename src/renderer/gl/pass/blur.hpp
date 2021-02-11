#ifndef __VTX_PASS_BLUR__
#define __VTX_PASS_BLUR__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_pass.hpp"

namespace VTX::Renderer::GL::Pass
{
	class Blur : public BasePass
	{
	  public:
		Blur( OpenGLFunctions * const p_gl ) : BasePass( p_gl ) {}
		virtual ~Blur();

		void init( const uint, const uint, const GL & ) override;
		void resize( const uint, const uint, const GL & ) override;
		void render( const Object3D::Scene &, const GL & ) override;

		inline const GLuint & getTexture() const { return _texture; }

		void clearTexture();

	  private:
		Program * _program				   = nullptr;
		GLuint	  _fboFirstPass			   = GL_INVALID_VALUE;
		GLuint	  _fbo					   = GL_INVALID_VALUE;
		GLuint	  _textureFirstPass		   = GL_INVALID_VALUE;
		GLuint	  _texture				   = GL_INVALID_VALUE;
		GLint	  _uBlurSizeLoc			   = GL_INVALID_INDEX;
		GLint	  _uInvDirectionTexSizeLoc = GL_INVALID_INDEX;
	};
} // namespace VTX::Renderer::GL::Pass

#endif
