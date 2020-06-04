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

				inline const GLuint & getBlurTexture() const { return _texture; }

				void clearTexture()
				{
					float clearColor[ 4 ] = { 0.f, 0.f, 0.f, 1.f };
					glClearTexImage( _texture, 0, GL_RGBA, GL_UNSIGNED_BYTE, &clearColor );
				}

			  private:
				GLSL::Program * _blurShader				 = nullptr;
				GLuint			_fboTmp					 = GL_INVALID_VALUE;
				GLuint			_fbo					 = GL_INVALID_VALUE;
				GLuint			_textureTmp				 = GL_INVALID_VALUE;
				GLuint			_texture				 = GL_INVALID_VALUE;
				GLint			_uBlurSizeLoc			 = GL_INVALID_INDEX;
				GLint			_uBlurSharpnessLoc		 = GL_INVALID_INDEX;
				GLint			_uInvDirectionTexSizeLoc = GL_INVALID_INDEX;
			};
		} // namespace Pass

	} // namespace Renderer
} // namespace VTX

#endif
