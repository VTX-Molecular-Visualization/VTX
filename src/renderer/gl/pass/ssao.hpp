#ifndef __VTX_PASS_SSAO__
#define __VTX_PASS_SSAO__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_pass.hpp"
#include "renderer/gl/texture_2d.hpp"

namespace VTX::Renderer::GL::Pass
{
	class SSAO : public BasePass
	{
	  public:
		SSAO( OpenGLFunctions * const p_gl ) : BasePass( p_gl ), _texture( p_gl ) {}
		virtual ~SSAO();

		void init( const uint, const uint, const GL & ) override;
		void resize( const uint, const uint, const GL & ) override;
		void render( const Object3D::Scene &, const GL & ) override;

		inline const GLuint getTexture() const { return _texture.getId(); }

	  private:
		Program * _program = nullptr;
		GLuint	  _fbo	   = GL_INVALID_VALUE;
		Texture2D _texture;
		GLuint	  _noiseTexture		= GL_INVALID_VALUE;
		GLint	  _uProjMatrixLoc	= GL_INVALID_INDEX;
		GLint	  _uAoKernelLoc		= GL_INVALID_INDEX;
		GLint	  _uKernelSizeLoc	= GL_INVALID_INDEX;
		GLint	  _uAoIntensityLoc	= GL_INVALID_INDEX;
		GLint	  _uNoiseSizeLoc	= GL_INVALID_INDEX;
		uint	  _kernelSize		= 16;
		uint	  _noiseTextureSize = 64;
	};
} // namespace VTX::Renderer::GL::Pass

#endif
