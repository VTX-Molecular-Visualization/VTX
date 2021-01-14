#ifndef __VTX_PASS_GEOMETRIC__
#define __VTX_PASS_GEOMETRIC__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_pass.hpp"

namespace VTX::Renderer::GL::Pass
{
	class Geometric : public BasePass
	{
	  public:
		Geometric( OpenGLFunctions * const p_gl ) : BasePass( p_gl ) {}
		virtual ~Geometric();
		virtual void init( ProgramManager &, const uint, const uint ) override;
		virtual void resize( const uint, const uint ) override;
		virtual void render( const Object3D::Scene &, const GL & ) override;

		inline const GLuint & getViewPositionsNormalsCompressedTexture() const
		{
			return _viewPositionsNormalsCompressedTexture;
		}
		inline const GLuint & getColorsTexture() const { return _colorsTexture; }
		inline const GLuint & getDepthTexture() const { return _depthTexture; }

	  private:
		GLuint _fbo									  = GL_INVALID_VALUE;
		GLuint _viewPositionsNormalsCompressedTexture = GL_INVALID_VALUE;
		GLuint _colorsTexture						  = GL_INVALID_VALUE;
		GLuint _depthTexture						  = GL_INVALID_VALUE;
	};
} // namespace VTX::Renderer::GL::Pass

#endif
