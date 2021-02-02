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

		void init( const uint, const uint, const GL & ) override;
		void resize( const uint, const uint, const GL & ) override;
		void render( const Object3D::Scene &, const GL & ) override;

		inline const GLuint & getTexture() const { return _texture; }

	  private:
		Program * _program = nullptr;
		GLuint	  _texture = GL_INVALID_VALUE;
	};
} // namespace VTX::Renderer::GL::Pass

#endif
