#ifndef __VTX_RENDER_RENDERER_GL_PASS_GEOMETRIC__
#define __VTX_RENDER_RENDERER_GL_PASS_GEOMETRIC__

#include "app/old/render/renderer/gl/framebuffer.hpp"
#include "app/old/render/renderer/gl/texture_2d.hpp"
#include "base_pass.hpp"

namespace VTX::App::Old::Render::Renderer::GL::Pass
{
	class Geometric : public BasePass
	{
	  public:
		Geometric()			 = default;
		virtual ~Geometric() = default;

		void init( const uint, const uint, const GL & ) override;
		void resize( const uint, const uint, const GL & ) override;
		void render( const App::Old::Application::Scene &, const GL & ) override;

		inline const Framebuffer & getFbo() const { return _fbo; }
		inline const Texture2D &   getViewPositionsNormalsCompressedTexture() const
		{
			return _viewPositionsNormalsCompressedTexture;
		}
		inline const Texture2D & getColorsTexture() const { return _colorsTexture; }
		inline const Texture2D & getDepthTexture() const { return _depthTexture; }

	  private:
		Framebuffer _fbo								   = Framebuffer();
		Texture2D	_viewPositionsNormalsCompressedTexture = Texture2D();
		Texture2D	_colorsTexture						   = Texture2D();
		Texture2D	_depthTexture						   = Texture2D();
		Texture2D	_pickingTexture						   = Texture2D();
	};
} // namespace VTX::App::Old::Render::Renderer::GL::Pass

#endif
