#ifndef __VTX_PASS_GEOMETRIC__
#define __VTX_PASS_GEOMETRIC__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_pass.hpp"
#include "renderer/gl/framebuffer.hpp"
#include "renderer/gl/texture_2d.hpp"

namespace VTX::Renderer::GL::Pass
{
	class Geometric : public BasePass
	{
	  public:
		Geometric( OpenGLFunctions * const p_gl ) :
			BasePass( p_gl ), _fbo( p_gl ), _viewPositionsNormalsCompressedTexture( p_gl ), _colorsTexture( p_gl ),
			_depthTexture( p_gl )
		{
		}
		virtual ~Geometric() = default;

		void init( const uint, const uint, const GL & ) override;
		void resize( const uint, const uint, const GL & ) override;
		void render( const Object3D::Scene &, const GL & ) override;

		inline const GLuint getViewPositionsNormalsCompressedTexture() const
		{
			return _viewPositionsNormalsCompressedTexture.getId();
		}
		inline const GLuint getColorsTexture() const { return _colorsTexture.getId(); }
		inline const GLuint getDepthTexture() const { return _depthTexture.getId(); }

	  private:
		Framebuffer _fbo;
		Texture2D	_viewPositionsNormalsCompressedTexture;
		Texture2D	_colorsTexture;
		Texture2D	_depthTexture;
	};
} // namespace VTX::Renderer::GL::Pass

#endif
