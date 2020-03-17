#ifndef __VTX_PASS_BLUR__
#define __VTX_PASS_BLUR__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_pass.hpp"

namespace VTX
{
	namespace Renderer
	{
		namespace Pass
		{
			class Blur : public BasePass
			{
			  public:
				virtual void init( GLSL::ProgramManager &, const uint, const uint ) override;
				virtual void clean() override;
				virtual void render( const Object3D::Scene &, const Renderer::GL & ) override;

				inline const GLuint & getBlurTexture() const { return _blurTexture; }

			  private:
				GLSL::Program * _blurShader	  = nullptr;
				GLuint			_fboBlur	  = GL_INVALID_VALUE;
				GLuint			_blurTexture  = GL_INVALID_VALUE;
				GLint			_uBlurSizeLoc = GL_INVALID_INDEX;
			};
		} // namespace Pass

	} // namespace Renderer
} // namespace VTX

#endif
