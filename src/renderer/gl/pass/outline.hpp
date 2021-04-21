#ifndef __VTX_PASS_OUTLINE__
#define __VTX_PASS_OUTLINE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_pass.hpp"

namespace VTX::Renderer::GL::Pass
{
	class Outline : public BasePass
	{
	  public:
		Outline( OpenGLFunctions * const p_gl ) : BasePass( p_gl ) {}
		virtual ~Outline();

		void init( const uint, const uint, const GL & ) override;
		void resize( const uint, const uint, const GL & ) override;
		void render( const Object3D::Scene &, const GL & ) override;

		inline const GLuint & getTexture() const { return _texture; }
		inline const GLuint & getFbo() const { return _fbo; }

	  private:
		GLuint	  _fbo	   = GL_INVALID_VALUE;
		GLuint	  _texture = GL_INVALID_VALUE;
		Program * _program = nullptr;

		GLint _uLineColorLoc = GL_INVALID_INDEX;
	};
} // namespace VTX::Renderer::GL::Pass

#endif
