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
				SSAO() = default;
				virtual ~SSAO();
				virtual void init( GLSL::ProgramManager &, const uint, const uint ) override;
				virtual void resize( const uint, const uint ) override;
				virtual void render( const Object3D::Scene &, const Renderer::GL & ) override;

				inline const GLuint & getTexture() const { return _texture; }

			  private:
				GLSL::Program * _program		  = nullptr;
				GLuint			_fbo			  = GL_INVALID_VALUE;
				GLuint			_texture		  = GL_INVALID_VALUE;
				GLuint			_noiseTexture	  = GL_INVALID_VALUE;
				GLint			_uProjMatrixLoc	  = GL_INVALID_INDEX;
				GLint			_uAoKernelLoc	  = GL_INVALID_INDEX;
				GLint			_uKernelSizeLoc	  = GL_INVALID_INDEX;
				GLint			_uAoIntensityLoc  = GL_INVALID_INDEX;
				GLint			_uNoiseSizeLoc	  = GL_INVALID_INDEX;
				uint			_kernelSize		  = 16;
				uint			_noiseTextureSize = 64;
			};
		} // namespace Pass

	} // namespace Renderer
} // namespace VTX

#endif
