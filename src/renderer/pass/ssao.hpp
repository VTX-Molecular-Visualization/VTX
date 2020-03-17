#ifndef __VTX_PASS_SSAO__
#define __VTX_PASS_SSAO__

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
			class SSAO : public BasePass
			{
			  public:
				virtual void init( GLSL::ProgramManager &, const uint, const uint ) override;
				virtual void clean() override;
				virtual void render( const Object3D::Scene &, const Renderer::GL & ) override;

				inline const GLuint & getSSAOTexture() const { return _ssaoTexture; }

			  private:
				GLSL::Program * _ssaoShader		  = nullptr;
				GLuint			_fboSSAO		  = GL_INVALID_VALUE;
				GLuint			_ssaoTexture	  = GL_INVALID_VALUE;
				GLuint			_noiseTexture	  = GL_INVALID_VALUE;
				GLint			_uProjMatrixLoc	  = GL_INVALID_INDEX;
				GLint			_uAoKernelLoc	  = GL_INVALID_INDEX;
				GLint			_uAoRadiusLoc	  = GL_INVALID_INDEX;
				GLint			_uKernelSizeLoc	  = GL_INVALID_INDEX;
				GLint			_uAoIntensityLoc  = GL_INVALID_INDEX;
				GLint			_kernelSize		  = 32;
				GLuint			_noiseTextureSize = 9;
			};
		} // namespace Pass

	} // namespace Renderer
} // namespace VTX

#endif
