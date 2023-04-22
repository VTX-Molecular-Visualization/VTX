#ifndef __VTX_RENDERER_GL_PASS_GEOMETRIC__
#define __VTX_RENDERER_GL_PASS_GEOMETRIC__

#include "base_pass.hpp"
#include "renderer/gl/framebuffer.hpp"
#include "renderer/gl/texture_2d.hpp"

namespace VTX::Renderer::GL::Pass
{
	class Geometric : public BasePass
	{
	  public:
		Geometric()			 = default;
		virtual ~Geometric() = default;

		void init( const size_t p_width, const size_t p_height ) override;
		void resize( const size_t p_width, const size_t p_height ) override;
		void render() override;

		/*
		inline const Framebuffer & getFbo() const { return _fbo; }
		inline const Texture2D &   getViewPositionsNormalsCompressedTexture() const
		{
			return _viewPositionsNormalsCompressedTexture;
		}
		inline const Texture2D & getColorsTexture() const { return _colorsTexture; }
		inline const Texture2D & getDepthTexture() const { return _depthTexture; }
		*/

	  private:
		Framebuffer _fbo								   = Framebuffer();
		Texture2D	_viewPositionsNormalsCompressedTexture = Texture2D();
		Texture2D	_colorsTexture						   = Texture2D();
		Texture2D	_depthTexture						   = Texture2D();
		Texture2D	_pickingTexture						   = Texture2D();
	};
} // namespace VTX::Renderer::GL::Pass

#endif
