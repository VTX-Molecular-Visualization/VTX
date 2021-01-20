#ifndef __VTX_PASS_FXAA__
#define __VTX_PASS_FXAA__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_pass.hpp"

namespace VTX::Renderer::GL::Pass
{
	class FXAA : public BasePass
	{
	  public:
		FXAA( OpenGLFunctions * const p_gl ) : BasePass( p_gl ) {}
		virtual ~FXAA();
		virtual void init( const uint, const uint ) override;
		virtual void resize( const uint, const uint ) override;
		virtual void render( const Object3D::Scene &, const GL & ) override;

		inline const GLuint & getTexture() const { return _texture; }
		inline const GLuint & getFbo() const { return _fbo; }

	  private:
		Program * _program = nullptr;
		GLuint	  _fbo	   = GL_INVALID_VALUE;
		GLuint	  _texture = GL_INVALID_VALUE;
	};
} // namespace VTX::Renderer::GL::Pass

#endif
